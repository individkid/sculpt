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
extern "C" {
#include "arithmetic.h"
#include "callback.h"
}

extern Window *window;

static Pool<Data> datas;
static Power<float> floats;

extern "C" void sendTopology(int file, int plane, float *point, enum Configure conf)
{
    Data *data = datas.get(); data->file = file; data->plane = plane;
    data->thread = WindowType; data->conf = conf;
    if (conf == RefineConf) {data->matrix = floats.get(3);
    for (int i = 0; i < 3; i++) data->matrix[i] = point[i];}
    window->sendData(PolytopeType,data);
}

extern "C" void sendWrite(int file, int plane, float *matrix, enum Configure conf)
{
    Data *data = datas.get(); data->file = file; data->plane = plane;
    data->thread = WindowType; data->conf = conf;
    data->matrix = floats.get(3);
    for (int i = 0; i < 3; i++) data->matrix[i] = matrix[i];
    window->sendData(WriteType,data);
}

extern "C" void warpCursor(float *cursor)
{
    window->warpCursor(cursor);
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
    if (buffer.target == GL_TEXTURE_2D) allocTexture2d(update); else {
    glBindBuffer(buffer.target,buffer.handle);
    glBufferData(buffer.target,update.size,NULL,buffer.usage);
    glBindBuffer(buffer.target,0);}
}

void Window::writeBuffer(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    if (update.function) update.function(update.file,&update);
    if (buffer.target == GL_TEXTURE_2D) writeTexture2d(update); else {
    glBindBuffer(buffer.target,buffer.handle);
    glBufferSubData(buffer.target,update.offset,update.size,update.data);
    glBindBuffer(buffer.target,0);}
}

void Window::bindBuffer(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    if (buffer.target == GL_TEXTURE_2D) bindTexture2d(update); else
    if (buffer.target == GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN)
    glBeginQuery(buffer.target,buffer.handle); else
    glBindBufferBase(buffer.target,buffer.index,buffer.handle);
}

void Window::unbindBuffer(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    if (buffer.target == GL_TEXTURE_2D) unbindTexture2d(); else
    if (buffer.target == GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN)
    glEndQuery(buffer.target); else
    glBindBufferBase(buffer.target,buffer.index,0);
}

void Window::readBuffer(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    if (buffer.target == GL_TEXTURE_2D) ; else
    if (buffer.target == GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN)
                                             glGetQueryObjectuiv(update.handle, GL_QUERY_RESULT, update.query); else {
    glBindBuffer(buffer.target,buffer.handle);
    glGetBufferSubData(buffer.target,update.offset,update.size,update.data);
    glBindBuffer(buffer.target,0);}
    if (update.function) update.function(update.file,&update);
}

void Window::allocTexture2d(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    glGenTextures(1,&update.handle);
    glActiveTexture(buffer.unit);
    glBindTexture(GL_TEXTURE_2D,update.handle);
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
    glBindTexture(GL_TEXTURE_2D,update.handle);
    if (update.function) update.function(update.file,&update);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, update.width, update.height, 0, GL_RGB, GL_UNSIGNED_BYTE, update.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);
}

void Window::bindTexture2d(Update &update)
{
    Handle &buffer = object[update.file].handle[update.buffer];
    glActiveTexture(buffer.handle);
    glBindTexture(GL_TEXTURE_2D,update.handle);
}

void Window::unbindTexture2d()
{
    glBindTexture(GL_TEXTURE_2D,0);
}

void Window::processResponse(Data &data)
{
    if (data.matrix) floats.put(16,data.matrix); datas.put(&data);
}

void Window::processData(Data &data)
{
    if (data.conf == TestConf) printf("%s",data.text);
    else syncMatrix(&data);
    switch (data.thread) {
    case (ReadType): object[data.file].read->reuse.put(&data); break;
    default: error("processData",data.thread,__FILE__,__LINE__); break;}
    glfwPollEvents();
}

void Window::startQueue(Queue &queue)
{
    Command *next = 0;
    while (next != queue.loop) {
    next = queue.first;
    if (!next->finish) processCommand(*next);
    if (next->finish) finishCommand(*next);
    glfwPollEvents();
    if (next->finish) break;
    deque(queue.first,queue.last);
    enque(queue.first,queue.last,next);}
}

void Window::finishQueue(Queue &queue)
{
    Command *next = 0;
    while (next != queue.loop) {
    next = deque(queue.first,queue.last);
    if (!next->finish) processCommand(*next);
    if (next->finish) finishCommand(*next);
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

void Window::startCommand(Queue &queue, Command &command)
{
    Command *next = &command;
    while (next) {
    if (!next->finish) processCommand(*next);
    if (next->finish) finishCommand(*next);
    glfwPollEvents();
    if (next->finish) enque(queue.first,queue.last,next);    
    next = next->next;}
}

void Window::processCommand(Command &command)
{
    for (Update *next = command.update[AllocField]; next; next = next->next) allocBuffer(*next);
    for (Update *next = command.update[WriteField]; next; next = next->next) writeBuffer(*next);
    for (Render *next = command.render; next; next = next->next) {
    Render &render = *next; Microcode &program = microcode[render.program];
    glUseProgram(program.handle); glBindVertexArray(object[render.file].vao[render.program][render.space]);
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
    if (command.pierce && pierce.last != pierce.loop) return;
    if (command.pierce && command.pierce->finish) return;
    if (command.pierce) swapQueue(pierce,command.pierce);
    if (command.redraw) swapQueue(redraw,command.redraw);
    for (Response *next = command.response; next; next = next->next)
    switch (next->thread) {
    case (ReadType): object[next->file].read->response.put(&command); break;
    case (PolytopeType): object[next->file].polytope->response.put(&command); break;
    default: error("finishCommand",next->thread,__FILE__,__LINE__); break;}
    command.finish = 0;
}

Window::Window(int n) : Thread(1), window(0), nfile(n), object(new Object[n]), response(this), data(this), request(this)
{
}

void Window::connect(int i, Write *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    object[i].write = ptr;
}

void Window::connect(int i, Polytope *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    object[i].polytope = ptr;
}

void Window::connect(int i, Read *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    object[i].read = ptr;
}

void Window::sendData(ThreadType thread, Data *data)
{
    switch (thread) {
    case (PolytopeType): object[data->file].polytope->action.put(data); break;
    case (WriteType): object[data->file].write->data.put(data); break;
    default: error("sendData",thread,__FILE__,__LINE__); break;}
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

void Window::call()
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
    glfwSetTime(0.0); while (testGoon) {double time = glfwGetTime();
    if (time < DELAY) {glfwWaitEventsTimeout(DELAY-time); continue;} glfwSetTime(0.0);
    finishQueue(query); if (isSuspend()) startQueue(pierce); else startQueue(redraw);
    Data *msg = 0; while (response.get(msg)) processResponse(*msg);
    msg = 0; while (data.get(msg)) processData(*msg);
    Command *command = 0; while (request.get(command)) startCommand(query,*command);}
    glfwTerminate();
}

void Window::wake()
{
	glfwPostEmptyEvent();
}
