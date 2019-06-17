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

void Window::connect(Polytope *ptr)
{
    rsp2polytope = &ptr->window2rsp;
    req2polytope = &ptr->window2req;
}

void Window::connect(Script *ptr)
{
    req2script = &ptr->window2req;
}

void Window::init()
{
    for (int i = 0; i < nfile; i++) if (object[i].rsp2command == 0) error("unconnected rsp2command",i,__FILE__,__LINE__);
    for (int i = 0; i < nfile; i++) if (object[i].rsp2read == 0) error("unconnected rsp2read",i,__FILE__,__LINE__);
    for (int i = 0; i < nfile; i++) if (object[i].req2write == 0) error("unconnected req2write",i,__FILE__,__LINE__);
    if (req2polytope == 0) error("unconnected req2polytope",0,__FILE__,__LINE__);
    if (rsp2polytope == 0) error("unconnected rsp2polytope",0,__FILE__,__LINE__);
    if (req2script == 0) error("unconnected req2script",0,__FILE__,__LINE__);
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
    if (DEBUG & WINDOW_DEBUG) std::cout << "OpenGL " << glGetString(GL_VERSION) << " GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
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
    processQueues(read);
    processQueues(polytope);
    processCommands(command2req,read);
    processCommands(polytope2req,polytope);
    processManips(polytope2rsp);
    processDatas(read2req);
    processDatas(write2rsp);
    processDatas(script2rsp);
}

void Window::wait()
{
    double time = glfwGetTime();
    if (time < DELAY) glfwWaitEventsTimeout(DELAY-time);
    else glfwSetTime(0.0);
}

void Window::wake()
{
    if (window) glfwPostEmptyEvent();
}

void Window::done()
{
    processQueues(read);
    processQueues(polytope);
    processCommands(command2req,read);
    processCommands(polytope2req,polytope);
    processDatas(read2req);
    processMacros();
    glfwTerminate(); window = 0;
}

Window::Window(int n) : Thread(1), object(new Object[n]),
    req2polytope(0), rsp2polytope(0), req2script(0),
    command2req(this,"Read->Command->Window"), read2req(this,"Read->Data->Window"),
    polytope2req(this,"Polytope->Data->Window"), polytope2rsp(this,"Window<-Manip<-Polytope"),
    write2rsp(this,"Window<-Data<-Write"), script2rsp(this,"Window->Data->Script"),
    window(0), finish(0), nfile(n)
{
    Object obj = {0}; for (int i = 0; i < n; i++) object[i] = obj;
    Queues que = {0}; read = polytope = que;
}

Window::~Window()
{
    if (window) error("done not called",0,__FILE__,__LINE__);
    processManips(polytope2rsp);
    processDatas(write2rsp);
    processDatas(script2rsp);
}

extern Window *window;

static Pool<Data> datas(__FILE__,__LINE__);
static Power<float> floats(__FILE__,__LINE__);
static std::map<std::pair<int,int>,Data*> macros;
static Pool<Manip> manips(__FILE__,__LINE__);

extern "C" void sendData(int file, int plane, enum Configure conf, float *matrix)
{
    Data *data = datas.get(); data->matrix = floats.get(16);
    data->file = file; data->plane = plane; data->conf = conf;
    for (int i = 0; i < 16; i++) data->matrix[i] = matrix[i];
    window->sendWrite(data);
}

extern "C" void sendSculpt(int file, int plane, enum ClickMode click)
{
    Manip *manip = manips.get();
    manip->file = file; manip->plane = plane; manip->click = click;
    window->sendPolytope(manip);
}

extern "C" void sendRefine(int file, int plane, float *vector)
{
    Manip *manip = manips.get(); manip->vector = floats.get(3);
    manip->file = file; manip->plane = plane; manip->click = RefineMode;
    for (int i = 0; i < 3; i++) manip->vector[i] = vector[i];
    window->sendPolytope(manip);
}

extern "C" void sendRelative(int file, int plane, enum TopologyMode topology, float *vector)
{
    Manip *manip = manips.get(); manip->vector = floats.get(3);
    manip->file = file; manip->plane = plane; manip->click = TweakMode;
    manip->topology = topology; manip->fixed = RelativeMode;
    for (int i = 0; i < 3; i++) manip->vector[i] = vector[i];
    window->sendPolytope(manip);
}

extern "C" void sendAbsolute(int file, int plane, enum TopologyMode topology)
{
    Manip *manip = manips.get();
    manip->file = file; manip->plane = plane; manip->click = TweakMode;
    manip->topology = topology; manip->fixed = AbsoluteMode;
    window->sendPolytope(manip);
}

extern "C" void sendFacet(int file, int plane, float *matrix)
{
    Manip *manip = manips.get(); manip->matrix = floats.get(16);
    manip->file = file; manip->plane = plane; manip->click = TransformMode;
    for (int i = 0; i < 16; i++) manip->matrix[i] = matrix[i];
    window->sendPolytope(manip);
}

extern "C" void sendInvoke(int file, int plane)
{
    std::pair<int,int> pair; pair.first = file; pair.second = plane;
    if (macros.find(pair) == macros.end()) error("invalid tagbits",0,__FILE__,__LINE__);
    window->sendScript(macros[pair]);
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

void Window::sendWrite(Data *data)
{
    object[data->file].req2write->put(data);
}

void Window::sendPolytope(Manip *manip)
{
    req2polytope->put(manip);
}

void Window::sendScript(Data *data)
{
    req2script->put(data);
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
    case (Inquery): glBeginQuery(buffer.target,buffer.handle); break;
    default: {
    glBindBufferBase(buffer.target,buffer.index,buffer.handle);
    break;}}
}

void Window::unbindBuffer(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    switch(update.buffer) {
    case (Texture0): case (Texture1): unbindTexture2d(); break;
    case (Inquery): glEndQuery(buffer.target); break;
    default: glBindBufferBase(buffer.target,buffer.index,0); break;}
}

void Window::readBuffer(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    switch(update.buffer) {
    case (Texture0): case (Texture1): break;
    case (Inquery): glGetQueryObjectuiv(buffer.handle, GL_QUERY_RESULT, update.query); break;
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
    int islast = (queue.loop == queue.last);
    if (command->next != 0) error("unsupported next",command->next,__FILE__,__LINE__);
    Command *next = 0;
    for (next = queue.first; next; next = next->next)
    if (next == command) {remove(queue.first,queue.last,next); break;}
    if (next == 0)
    for (next = queue.first; next; next = next->next)
    if (next->file == command->file) {
    Command *temp = next; remove(queue.first,queue.last,next);
    enque(queue.first,queue.last,command); command = next; break;}
    enque(queue.first,queue.last,command); command = 0;
    if (islast) queue.loop = queue.last;
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

void Window::finishCommand(Command &command, Queues &queues)
{
    for (Update *next = command.update[ReadField]; next; next = next->next) {
    if (next->finish) readBuffer(*next);
    if (next->finish) return;}
    if (command.pierce && queues.pierce.last != queues.pierce.loop) return;
    if (command.pierce && command.pierce->finish) return;
    if (command.pierce) swapQueue(queues.pierce,command.pierce);
    if (command.redraw) swapQueue(queues.redraw,command.redraw);
    command.finish = 0;
}

void Window::processQueues(Queues &queues)
{
    if (isSuspend()) processQueue(queues.pierce,queues);
    else processQueue(queues.redraw,queues);
    processQueue(queues.query,queues);
}

void Window::processQueue(Queue &queue, Queues &queues)
{
    Command *command = queue.first; while (command) {
        Command *temp = command->next;
        if (!command->finish) startCommand(*command);
        if (command->finish) finishCommand(*command,queues);
        if (!command->finish) {
            remove(queue.first,queue.last,command);
            if (&queue == &queues.query) {
                if (&queues == &read) object[command->file].rsp2command->put(command);
                if (&queues == &polytope) rsp2polytope->put(command);}
            else enque(queue.first,queue.last,command);}
        if (command->finish && &queue != &queues.query) break;
        if (command == queue.loop && window) break;
        command = temp;}
    if (&queue == &queues.query) queue.loop = queue.last;
}

void Window::processCommands(Message<Command> &message, Queues &queues)
{
    Command *command; while (message.get(command)) {
        if (command->next != 0) error("unsupported next",command->next,__FILE__,__LINE__);
        if (!command->finish) startCommand(*command);
        if (command->finish) finishCommand(*command,queues);
        if (command->finish) {
            if (&message == &command2req) enque(read.query.first,read.query.last,command);
            if (&message == &polytope2req) enque(polytope.query.first,polytope.query.last,command);}
        else {
            if (&message == &command2req) object[command->file].rsp2command->put(command);}}
}

void Window::processManips(Message<Manip> &message)
{
    Manip *manip; while (message.get(manip)) {
        switch (manip->click) {
        case (RefineMode): floats.put(3,manip->vector); break;
        case (TweakMode): switch (manip->fixed) {
        case (RelativeMode): floats.put(3,manip->vector); break;
        case (AbsoluteMode): break;
        default: error("invalid fixed",manip->fixed,__FILE__,__LINE__);}
        case (TransformMode): floats.put(16,manip->matrix); break;
        default: error("invalid click",manip->click,__FILE__,__LINE__);}
        manips.put(manip);}
}

void Window::processDatas(Message<Data> &message)
{
    Data *data; while (message.get(data)) {
        if (&message == &read2req) {
            if (data->conf == MacroConf) {
                std::pair<int,int> pair; pair.first = data->file; pair.second = data->plane;
                if (macros.find(pair) != macros.end()) object[data->file].rsp2read->put(macros[pair]);
                macros[pair] = data;}
            else {
                changeState(data);
                object[data->file].rsp2read->put(data);}}
        if (&message == &write2rsp) {
            floats.put(16,data->matrix);
            datas.put(data);}
        if (&message == &script2rsp) {
            // nothing to do
        }}
}

void Window::processMacros()
{
    while (macros.begin() != macros.end()) {
    std::map<std::pair<int,int>,Data*>::iterator iter = macros.begin();
    Data *data = (*iter).second;
    macros.erase(iter);
    object[data->file].rsp2read->put(data);}
}
