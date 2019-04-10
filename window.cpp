/*
*    window.cpp start start opengl, wait for buffer changes and feedback requests
*    Copyright (C) 2019  Paul Coelho
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#ifdef __APPLE__
#define Point ApplePoint
#define Handle AppleHandle
#include <CoreGraphics/CoreGraphics.h>
#undef Point
#undef Handle
#endif

#include "window.hpp"
#include "object.hpp"
#include "write.hpp"
#include "polytope.hpp"
#include "read.hpp"
#include "script.hpp"
extern "C" {
#include "arithmetic.h"
#include "callback.h"
}

extern Window *window;

static Pool<Data> datas(__FILE__,__LINE__);
static Power<float> floats(__FILE__,__LINE__);

extern "C" void sendData(int file, int plane, enum Configure conf, float *matrix)
{
    Data *data = datas.get(); data->matrix = floats.get(16);
    data->file = file; data->plane = plane; data->conf = conf;
    for (int i = 0; i < 16; i++) data->matrix[i] = matrix[i];
    window->sendWrite(data);
}

extern "C" void sendAdditive(int file, int plane)
{
    Data *data = datas.get();
    data->file = file; data->plane = plane; data->conf = AdditiveConf;
    window->sendPolytope(data);
}

extern "C" void sendSubtracive(int file, int plane)
{
    Data *data = datas.get();
    data->file = file; data->plane = plane; data->conf = SubtractiveConf;
    window->sendPolytope(data);
}

extern "C" void sendRefine(int file, int plane, float *pierce)
{
    Data *data = datas.get(); data->pierce = floats.get(3);
    data->file = file; data->plane = plane; data->conf = RefineConf;
    for (int i = 0; i < 3; i++) data->pierce[i] = pierce[i];
    window->sendPolytope(data);
}

extern "C" void sendRelative(int file, int plane, enum TopologyMode topology, float *pierce)
{
    Data *data = datas.get(); data->pierce = floats.get(3);
    data->file = file; data->plane = plane; data->conf = TweakConf;
    data->topology = topology; data->fixed = RelativeMode;
    for (int i = 0; i < 3; i++) data->pierce[i] = pierce[i];
    window->sendPolytope(data);
}

extern "C" void sendAbsolute(int file, int plane, enum TopologyMode topology)
{
    Data *data = datas.get();
    data->file = file; data->plane = plane; data->conf = TweakConf;
    data->topology = topology; data->fixed = AbsoluteMode;
    window->sendPolytope(data);
}

extern "C" void sendFacet(int file, int plane, float *matrix)
{
    Data *data = datas.get(); data->matrix = floats.get(16);
    data->file = file; data->plane = plane; data->conf = TransformConf;
    for (int i = 0; i < 16; i++) data->matrix[i] = matrix[i];
    window->sendPolytope(data);
}

extern "C" void sendInvoke(int file, int plane, int tagbits)
{
    Data *data = datas.get(); data->tagbits = tagbits;
    data->file = file; data->plane = plane; data->conf = MacroConf;
    window->sendPolytope(data);
}

extern "C" void warpCursor(float *cursor)
{
    window->warpCursor(cursor);
}

extern "C" void maybeKill(int seq)
{
    window->maybeKill(seq);
}

extern "C" int decodeClick(int button, int action, int mods)
{
    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT && (mods & GLFW_MOD_CONTROL) != 0) return 0;
    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT && (mods & GLFW_MOD_CONTROL) != 0) return 1;
    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) return 1;
    return -1;
}

void Window::allocBuffer(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    switch(update.buffer) {
    case (Texture0): case (Texture1): allocTexture2d(update); break;
    default: {
    glBindBuffer(buffer.target,buffer.handle);
    glBufferData(buffer.target,update.size,NULL,buffer.usage);
    glBindBuffer(buffer.target,0);
    break;}}
}

void Window::writeBuffer(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    if (update.function) update.function(&update);
    switch(update.buffer) {
    case (Texture0): case (Texture1):  writeTexture2d(update); break;
    default: {
    glBindBuffer(buffer.target,buffer.handle);
    glBufferSubData(buffer.target,update.offset,update.size,update.data);
    glBindBuffer(buffer.target,0);
    break;}}
}

void Window::bindBuffer(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    switch(update.buffer) {
    case (Texture0): case (Texture1): bindTexture2d(update); break;
    case (Query): glBeginQuery(buffer.target,buffer.handle); break;
    default: {
    glBindBufferBase(buffer.target,buffer.index,buffer.handle);
    break;}}
}

void Window::unbindBuffer(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    switch(update.buffer) {
    case (Texture0): case (Texture1): unbindTexture2d(); break;
    case (Query): glEndQuery(buffer.target); break;
    default: glBindBufferBase(buffer.target,buffer.index,0); break;}
}

void Window::readBuffer(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    switch(update.buffer) {
    case (Texture0): case (Texture1): break;
    case (Query): glGetQueryObjectuiv(buffer.handle, GL_QUERY_RESULT, update.query); break;
    default: {
    glBindBuffer(buffer.target,buffer.handle);
    glGetBufferSubData(buffer.target,update.offset,update.size,update.data);
    glBindBuffer(buffer.target,0);
    break;}}
    if (update.function) update.function(&update);
}

void Window::allocTexture2d(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    glGenTextures(1,&buffer.handle);
    glActiveTexture(buffer.unit);
    glBindTexture(GL_TEXTURE_2D,buffer.handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);
}

void Window::writeTexture2d(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    glActiveTexture(buffer.handle);
    glBindTexture(GL_TEXTURE_2D,buffer.handle);
    if (update.function) update.function(&update);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, update.width, update.height, 0, GL_RGB, GL_UNSIGNED_BYTE, update.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);
}

void Window::bindTexture2d(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    glActiveTexture(buffer.handle);
    glBindTexture(GL_TEXTURE_2D,buffer.handle);
}

void Window::unbindTexture2d()
{
    glBindTexture(GL_TEXTURE_2D,0);
}

void Window::swapQueue(Queue &queue, Command *&command)
{
    if (command == queue.first) {
    queue.first = queue.last = queue.loop = 0;}
    else {Command *temp = queue.first;
    while (command) {
    enque(queue.first,queue.last,command);
    command = command->next;}
    queue.loop = queue.last; command = temp;}
}

void Window::startCommand(Command &command)
{
    for (Update *next = command.update[AllocField]; next; next = next->next) allocBuffer(*next);
    for (Update *next = command.update[WriteField]; next; next = next->next) writeBuffer(*next);
    for (Render *next = command.render; next; next = next->next) {
    Render &render = *next; Microcode &program = microcode[render.program];
    glUseProgram(program.handle); glBindVertexArray(object[render.file].vao[render.program]);
    for (Update *next = command.update[BindField]; next; next = next->next) bindBuffer(*next);
    if (command.feedback) {glEnable(GL_RASTERIZER_DISCARD); glBeginTransformFeedback(program.primitive);}
    if (render.count) glDrawElements(program.mode,render.count,GL_UNSIGNED_INT,reinterpret_cast<void*>(render.base));
    if (render.size) glDrawArrays(program.mode,render.base,render.size);
    if (command.feedback) {glEndTransformFeedback(); glDisable(GL_RASTERIZER_DISCARD);}
    for (Update *next = command.update[BindField]; next; next = next->next) unbindBuffer(*next);
    glBindVertexArray(0); glUseProgram(0);}
    if (command.feedback) glFlush(); else glfwSwapBuffers(window);
    for (Update *next = command.update[ReadField]; next; next = next->next) next->finish = 1;
    command.finish = 1;
}

void Window::finishCommand(Command &command)
{
    for (Update *next = command.update[ReadField]; next; next = next->next) {
    if (next->finish) readBuffer(*next);
    if (next->finish) return;}
    if (command.pierce && current->pierce.last != current->pierce.loop) return;
    if (command.pierce && command.pierce->finish) return;
    if (command.pierce) swapQueue(current->pierce,command.pierce);
    if (command.redraw) swapQueue(current->redraw,command.redraw);
    command.finish = 0;
}

void Window::processRedraw(Command *command, void (Window::*respond)(Command *command))
{
    if (!command->finish) startCommand(*command);
    if (command->finish) finishCommand(*command);
    if (!command->finish) {
    remove(current->redraw.first,current->redraw.last,command);
    enque(current->redraw.first,current->redraw.last,command);}
}

void Window::processPierce(Command *command, void (Window::*respond)(Command *command))
{
    if (!command->finish) startCommand(*command);
    if (command->finish) finishCommand(*command);
    if (!command->finish) {
    remove(current->pierce.first,current->pierce.last,command);
    enque(current->pierce.first,current->pierce.last,command);}
}

void Window::processQuery(Command *command, void (Window::*respond)(Command *command))
{
    if (!command->finish) startCommand(*command);
    if (command->finish) finishCommand(*command);
    if (!command->finish) {remove(current->query.first,current->query.last,command);
    (this->*respond)(command);}
}

void Window::processCommand(Command *command, void (Window::*respond)(Command *command))
{
    if (!command->finish) startCommand(*command);
    if (command->finish) finishCommand(*command);
    if (command->finish) enque(current->query.first,current->query.last,command);
    else (this->*respond)(command);
}

void Window::processRespond(Command *command, void (Window::*respond)(Command *command))
{
    (this->*respond)(command);
}

void Window::processRead(Data *data)
{
    changeState(data);
    respondRead(data);
}

void Window::respondCommand(Command *command)
{
    object[command->file].rsp2command->put(command);
}

void Window::respondRead(Data *data)
{
    object[data->file].rsp2read->put(data);
}

void Window::responsePolytope(Data *data)
{
    floats.put(16,data->matrix);
    datas.put(data);
}

void Window::respondPolytope(Command *command)
{
    object[command->file].rsp2polytope->put(command);
}

void Window::responseWrite(Data *data)
{
    switch (data->conf) {
    case (AdditiveConf): datas.put(data); break;
    case (SubtractiveConf): datas.put(data); break;
    case (RefineConf): floats.put(3,data->pierce); datas.put(data); break;
    case (TweakConf): switch (data->fixed) {
    case (RelativeMode): floats.put(3,data->pierce); datas.put(data); break;
    case (AbsoluteMode): datas.put(data); break;
    default: error("unexpected fixed",data->fixed,__FILE__,__LINE__);} break;
    case (TransformConf): floats.put(16,data->matrix); datas.put(data); break;
    case (MacroConf): datas.put(data); break;
    default: error("unexpected conf",data->conf,__FILE__,__LINE__);}
}

void Window::respondScript(Command *command)
{
    rsp2script->put(command);
}

void Window::processTwice(Queues &queues, void (Window::*respond)(Command *command))
{
    processQueues(queues,respond);
    processQueues(queues,respond);
}

void Window::processQueues(Queues &queues, void (Window::*respond)(Command *command))
{
    current = &queues;
    processQueue(queues.redraw,&Window::processRedraw,respond);
    processQueue(queues.pierce,&Window::processPierce,respond);
    processQueue(queues.query,&Window::processQuery,respond);
}

void Window::processQueue(Queue &queue, void (Window::*process)(Command *command,
    void (Window::*respond)(Command *command)), void (Window::*respond)(Command *command))
{
    while (queue.first) {
    Command *command = queue.first;
    (this->*process)(command,respond);
    if (command->finish) return;
    if (command == queue.loop) break;}
    queue.loop = queue.last;
}

void Window::processCommands(Message<Command*> &message, void (Window::*process)(Command *command,
    void (Window::*respond)(Command *command)), void (Window::*respond)(Command *command))
{
    Command *command; while (message.get(command)) (this->*process)(command,respond);
}

void Window::processDatas(Message<Data*> &message, void (Window::*process)(Data *data))
{
    Data *data; while (message.get(data)) (this->*process)(data);
}

Window::Window(int n) : Thread(1), window(0), finish(0), nfile(n), current(0), object(new Object[n]),
    command2req(this,"Read->Command->Window"), read2req(this,"Read->Data->Window"),
    polytope2rsp(this,"Window<-Data<-Polytope"), polytope2req(this,"Polytope->Data->Window"),
    write2rsp(this,"Window<-Data<-Write"), script2req(this,"Script->Data->Window")
{
    Queues init = {0}; script = command = polytope = init;
}

Window::~Window()
{
    processDatas(polytope2rsp,&Window::responsePolytope);
    processDatas(write2rsp,&Window::responseWrite);
}

void Window::connect(int i, Read *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    object[i].rsp2command = &ptr->command2rsp;
    object[i].rsp2read = &ptr->window2rsp;
}

void Window::connect(int i, Write *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    object[i].req2write = &ptr->window2req;
}

void Window::connect(int i, Polytope *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    object[i].req2polytope = &ptr->window2req;
    object[i].rsp2polytope = &ptr->window2rsp;
}

void Window::connect(Script *ptr)
{
    rsp2script = &ptr->window2rsp;
}

void Window::sendWrite(Data *data)
{
    object[data->file].req2write->put(data);
}

void Window::sendPolytope(Data *data)
{
    object[data->file].req2polytope->put(data);
}

void Window::warpCursor(float *cursor)
{
#ifdef __linux__
    // double xpos, ypos;
    // glfwGetCursorPos(window,&xpos,&ypos);
    // XWarpPointer(screenHandle,None,None,0,0,0,0,cursor[0]-xpos,cursor[1]-ypos);
#endif
#ifdef __APPLE__
    int xloc, yloc;
    glfwGetWindowPos(window,&xloc,&yloc);
    struct CGPoint point; point.x = xloc+cursor[0]; point.y = yloc+cursor[1];
    CGWarpMouseCursorPosition(point);
#endif
}

void Window::maybeKill(int seq)
{
    if (seq == 0) finish = 0;
    if (seq == 1 && finish == 0) finish = 1;
    if (seq == 2 && finish == 1) kill();
}

void Window::init()
{
    for (int i = 0; i < nfile; i++) if (object[i].rsp2command == 0) error("unconnected rsp2command",i,__FILE__,__LINE__);
    for (int i = 0; i < nfile; i++) if (object[i].rsp2read == 0) error("unconnected rsp2read",i,__FILE__,__LINE__);
    for (int i = 0; i < nfile; i++) if (object[i].rsp2polytope == 0) error("unconnected rsp2polytope",i,__FILE__,__LINE__);
    for (int i = 0; i < nfile; i++) if (object[i].req2polytope == 0) error("unconnected req2polytope",i,__FILE__,__LINE__);
    for (int i = 0; i < nfile; i++) if (object[i].req2write == 0) error("unconnected req2write",i,__FILE__,__LINE__);
    if (rsp2script == 0) error("unconnected rsp2script",0,__FILE__,__LINE__);
    if (sizeof(GLenum) != sizeof(MYenum)) error("sizeof enum",sizeof(GLenum),__FILE__,__LINE__);
    if (sizeof(GLuint) != sizeof(MYuint)) error("sizeof uint",sizeof(GLuint),__FILE__,__LINE__);
    if (sizeof(GLfloat) != sizeof(float)) error("sizeof float",sizeof(GLfloat),__FILE__,__LINE__);
    if (sizeof(float)%4 != 0) error("sizeof float",sizeof(float)%4,__FILE__,__LINE__);
    globalInit(nfile);
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSetErrorCallback(displayError);
    window = glfwCreateWindow(1024, 768, "SideGeo", NULL, NULL);
    if (!window) error("Failed to open GLFW window",0,__FILE__,__LINE__);
    glfwSetKeyCallback(window, displayKey);
    glfwSetCursorPosCallback(window, displayCursor);
    glfwSetScrollCallback(window, displayScroll);
    glfwSetMouseButtonCallback(window, displayClick);
    glfwMakeContextCurrent(window);
    if (gl3wInit()) error("Failed to initialize OpenGL",0,__FILE__,__LINE__);
    if (!gl3wIsSupported(3, 3)) error("OpenGL 3.3 not supported",0,__FILE__,__LINE__);
    if (DEBUG) std::cout << "OpenGL " << glGetString(GL_VERSION) << " GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window);
    for (Program p = (Program)0; p < Programs; p = (Program)((int)p+1)) microcode[p].initProgram(p);
    for (int f = 0; f < nfile; f++) object[f].initFile(f==0);
    glfwSetTime(0.0);
}

void Window::call()
{

    processQueues(script,&Window::respondScript);
    processQueues(command,&Window::respondCommand);
    processQueues(polytope,&Window::respondPolytope);
    processCommands(script2req,&Window::processCommand,&Window::respondScript);
    processCommands(command2req,&Window::processCommand,&Window::respondCommand);
    processCommands(polytope2req,&Window::processCommand,&Window::respondPolytope);
    processDatas(read2req,&Window::processRead);
}

void Window::wait()
{
    double time = glfwGetTime();
    if (time < DELAY) glfwWaitEventsTimeout(DELAY-time);
    else glfwSetTime(0.0);
}

void Window::wake()
{
    if (window)
	glfwPostEmptyEvent();
}

void Window::done()
{
    glfwTerminate();
    window = 0;
    processTwice(script,&Window::respondScript);
    processTwice(command,&Window::respondCommand);
    processTwice(polytope,&Window::respondPolytope);
    processCommands(script2req,&Window::processRespond,&Window::respondScript);
    processCommands(command2req,&Window::processRespond,&Window::respondCommand);
    processCommands(polytope2req,&Window::processRespond,&Window::respondPolytope);
    processDatas(read2req,&Window::respondRead);
}
