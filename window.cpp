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

static Pool<Data> datas;
static Power<float> floats;

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

void Window::processCommand(Command &command)
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

void Window::finishCommand(Command &command, Queues &queues)
{
    for (Update *next = command.update[ReadField]; next; next = next->next) {
    if (next->finish) readBuffer(*next);
    if (next->finish) return;}
    if (command.pierce && queues.pierce.last != queues.pierce.loop) return;
    if (command.pierce && command.pierce->finish) return;
    if (command.pierce) swapQueue(queues.pierce,command.pierce);
    if (command.redraw) swapQueue(queues.redraw,command.redraw);
    for (Response *next = command.response; next; next = next->next)
    command.finish = 0;
}

void Window::finishQueue(Queues &queues, void (Window::*response)(Command *command,int file))
{
    Command *next = 0;
    while (next != queues.query.loop) {
    next = deque(queues.query.first,queues.query.last);
    if (!next->finish) processCommand(*next);
    if (next->finish) finishCommand(*next,queues);
    glfwPollEvents();
    if (next->finish) enque(queues.query.first,queues.query.last,next);
    if (!next->finish)
    for (Response *resp = next->response; resp; resp = resp->next)
    (*this.*response)(next,resp->file);}
    queues.query.loop = queues.query.last;
}

void Window::startQueue(Queue &queue, Queues &queues)
{
    Command *next = 0;
    while (next != queue.loop) {
    next = queue.first;
    if (!next->finish) processCommand(*next);
    if (next->finish) finishCommand(*next,queues);
    glfwPollEvents();
    if (next->finish) break;
    deque(queue.first,queue.last);
    enque(queue.first,queue.last,next);}
}

void Window::processResponse(Data &data)
{
    // TODO
}

void Window::processData(Data &data)
{
    changeState(&data);
    object[data.file].rsp2read->put(&data);
    glfwPollEvents();
}

void Window::startCommand(Command &command, Queues &queues, void (Window::*response)(Command *command,int file))
{
    Command *next = &command;
    while (next) {
    if (!next->finish) processCommand(*next);
    if (next->finish) finishCommand(*next,queues);
    glfwPollEvents();
    if (next->finish) enque(queues.query.first,queues.query.last,next);    
    Command *temp = next; next = next->next;
    if (!temp->finish)
    for (Response *resp = temp->response; resp; resp = resp->next)
    (*this.*response)(temp,resp->file);}
}

void Window::startQueues(Queues &queues, void (Window::*response)(Command *command,int file))
{
    finishQueue(queues,response);
    if (isSuspend()) startQueue(queues.pierce,queues);
    else startQueue(queues.redraw,queues);
}

void Window::processResponses(Message<Data*> &response)
{
    Data *data;
    while (response.get(data))
    processResponse(*data);
}

void Window::processRequests(Message<Data*> &request)
{
    Data *data;
    while (request.get(data))
    processData(*data);
}

void Window::startCommands(Message<Command*> &request, Queues &queues, void (Window::*response)(Command *command,int file))
{
    Command *command;
    while (request.get(command))
    startCommand(*command,queues,response);
}

void Window::respondScript(Command *command, int file)
{
    rsp2script->put(command);
}

void Window::respondCommand(Command *command, int file)
{
    object[file].rsp2command->put(command);
}

void Window::respondPolytope(Command *command, int file)
{
    object[file].rsp2polytope->put(command);
}

Window::Window(int n) : Thread(1), window(0), finish(0), nfile(n), object(new Object[n]),
    command2req(this), read2req(this), write2rsp(this),
    polytope2rsp(this), polytope2req(this), script2req(this)
{
    Queues init = {0}; script = command = polytope = init;
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
    if (!window) {std::cerr << "Failed to open GLFW window" << std::endl; glfwTerminate(); exit(-1);}
    glfwSetKeyCallback(window, displayKey);
    glfwSetCursorPosCallback(window, displayCursor);
    glfwSetScrollCallback(window, displayScroll);
    glfwSetMouseButtonCallback(window, displayClick);
    glfwMakeContextCurrent(window);
    if (gl3wInit()) {std::cerr << "Failed to initialize OpenGL" << std::endl; exit(-1);}
    if (!gl3wIsSupported(3, 3)) {std::cerr << "OpenGL 3.3 not supported\n" << std::endl; exit(-1);}
    std::cout << "OpenGL " << glGetString(GL_VERSION) << " GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
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
    startQueues(script,&Window::respondScript);
    startQueues(command,&Window::respondCommand);
    startQueues(polytope,&Window::respondPolytope);
    processResponses(polytope2rsp);
    processResponses(write2rsp);
    processRequests(read2req);
    startCommands(script2req,script,&Window::respondScript);
    startCommands(command2req,command,&Window::respondCommand);
    startCommands(polytope2req,polytope,&Window::respondPolytope);
}

void Window::wait()
{
    double time = glfwGetTime();
    if (time < DELAY) glfwWaitEventsTimeout(DELAY-time);
    else glfwSetTime(0.0);
}

void Window::wake()
{
	glfwPostEmptyEvent();
}

void Window::done()
{
    glfwTerminate();
}
