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
static Pool<Action> actions;
static Pool<Invoke> invokes;
static Power<float> floats;

extern "C" void sendData(int file, int plane, enum Configure conf, float *matrix)
{
    Data *data = datas.get(); data->matrix = floats.get(9);
    data->file = file; data->plane = plane; data->conf = conf;
    for (int i = 0; i < 9; i++) data->matrix[i] = matrix[i];
    window->sendData(data);
}

extern "C" void sendAdditive(int file, int plane)
{
    Action *action = actions.get();
    action->file = file; action->plane = plane; action->click = AdditiveMode;
    window->sendAction(action);
}

extern "C" void sendSubtracive(int file, int plane)
{
    Action *action = actions.get();
    action->file = file; action->plane = plane; action->click = SubtractiveMode;
    window->sendAction(action);
}

extern "C" void sendRefine(int file, int plane, float *pierce)
{
    Action *action = actions.get(); action->pierce = floats.get(3);
    action->file = file; action->plane = plane; action->click = RefineMode;
    for (int i = 0; i < 3; i++) action->pierce[i] = pierce[i];
    window->sendAction(action);
}

extern "C" void sendRelative(int file, int plane, enum TopologyMode topology, float *pierce)
{
    Action *action = actions.get(); action->pierce = floats.get(3);
    action->file = file; action->plane = plane; action->click = TweakMode;
    action->topology = topology; action->fixed = RelativeMode;
    for (int i = 0; i < 3; i++) action->pierce[i] = pierce[i];
    window->sendAction(action);
}

extern "C" void sendAbsolute(int file, int plane, enum TopologyMode topology)
{
    Action *action = actions.get();
    action->file = file; action->plane = plane; action->click = TweakMode;
    action->topology = topology; action->fixed = AbsoluteMode;
    window->sendAction(action);
}

extern "C" void sendFacet(int file, int plane, float *matrix)
{
    Action *action = actions.get(); action->matrix = floats.get(9);
    action->file = file; action->plane = plane; action->click = TransformMode;
    for (int i = 0; i < 9; i++) action->matrix[i] = matrix[i];
    window->sendAction(action);
}

extern "C" void sendInvoke(int file, int plane, int tagbits, float *vector)
{
    Invoke *invoke = invokes.get(); invoke->vector = floats.get(3);
    invoke->file = file; invoke->plane = plane; invoke->tagbits = tagbits;
    for (int i = 0; i < 3; i++) invoke->vector[i] = vector[i];
    window->sendInvoke(invoke);
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

void Window::processResponse(Invoke &invoke)
{
}

void Window::processResponse(Action &action)
{
}

void Window::processResponse(Data &data)
{
}

void Window::processData(Data &data)
{
    syncMatrix(&data);
    object[data.file].rsp2data2read->put(&data);
    glfwPollEvents();
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

void Window::finishCommand(Command &command, Message<Command*> *Object::*response)
{
    for (Update *next = command.update[ReadField]; next; next = next->next) {
    if (next->finish) readBuffer(*next);
    if (next->finish) return;}
    if (command.pierce && pierce.last != pierce.loop) return;
    if (command.pierce && command.pierce->finish) return;
    if (command.pierce) swapQueue(pierce,command.pierce);
    if (command.redraw) swapQueue(redraw,command.redraw);
    for (Response *next = command.response; next; next = next->next)
    (object[next->file].*response)->put(&command);
    command.finish = 0;
}

void Window::startCommand(Queue &queue, Command &command, Message<Command*> *Object::*response)
{
    Command *next = &command;
    while (next) {
    if (!next->finish) processCommand(*next);
    if (next->finish) finishCommand(*next,response);
    glfwPollEvents();
    if (next->finish) enque(queue.first,queue.last,next);    
    next = next->next;}
}

void Window::startQueue(Queue &queue, Message<Command*> *Object::*response)
{
    Command *next = 0;
    while (next != queue.loop) {
    next = queue.first;
    if (!next->finish) processCommand(*next);
    if (next->finish) finishCommand(*next,response);
    glfwPollEvents();
    if (next->finish) break;
    deque(queue.first,queue.last);
    enque(queue.first,queue.last,next);}
}

void Window::finishQueue(Queue &queue, Message<Command*> *Object::*response)
{
    Command *next = 0;
    while (next != queue.loop) {
    next = deque(queue.first,queue.last);
    if (!next->finish) processCommand(*next);
    if (next->finish) finishCommand(*next,response);
    glfwPollEvents();
    if (next->finish) enque(queue.first,queue.last,next);}
    queue.loop = queue.last;
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

Window::Window(int n) : Thread(1), window(0), finish(0), nfile(n), object(new Object[n]),
    read2command2req(this), read2data2req(this), write2data2rsp(this),
    polytope2action2rsp(this), script2invoke2rsp(this), polytope2command2req(this)
{
    Queue init = {0}; redraw = pierce = query = init;
}

void Window::connect(int i, Read *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    object[i].rsp2command2read = &ptr->window2command2rsp;
    object[i].rsp2data2read = &ptr->window2data2rsp;
}

void Window::connect(int i, Write *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    object[i].req2data2write = &ptr->window2data2req;
}

void Window::connect(int i, Polytope *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    object[i].req2action2polytope = &ptr->window2action2req;
    object[i].rsp2command2polytope = &ptr->window2command2rsp;
}

void Window::connect(Script *ptr)
{
    req2invoke2script = &ptr->window2invoke2req;
}

void Window::sendData(Data *data)
{
    object[data->file].req2data2write->put(data);
}

void Window::sendAction(Action *action)
{
    object[action->file].req2action2polytope->put(action);
}

void Window::sendInvoke(Invoke *invoke)
{
    req2invoke2script->put(invoke);
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
    finishQueue(query,&Object::rsp2command2read);
    if (isSuspend()) startQueue(pierce,&Object::rsp2command2read);
    else startQueue(redraw,&Object::rsp2command2read);
    Invoke *response = 0; while (script2invoke2rsp.get(response)) processResponse(*response);
    Action *action = 0; while (polytope2action2rsp.get(action)) processResponse(*action);
    Data *data = 0; while (write2data2rsp.get(data)) processResponse(*data);
    data = 0; while (read2data2req.get(data)) processData(*data);
    Command *command = 0; while (read2command2req.get(command)) startCommand(query,*command,&Object::rsp2command2read);
    command = 0; while (polytope2command2req.get(command)) startCommand(query,*command,&Object::rsp2command2polytope);
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
