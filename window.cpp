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

extern "C" void sendAction(struct Rawdata *rawdata)
{
    window->sendAction(rawdata);
}

extern "C" void sendData(struct Rawdata *rawdata)
{
    window->sendData(rawdata);
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

int Window::compare(const char *pre, std::string str, std::string &res)
{
    int len = strlen(pre);
    if (str.substr(0,len) != pre) return 0;
    res = str.substr(len,std::string::npos); return 1;
}

Window::Window(int n) : Thread(1), window(0), nfile(n), object(new Object[n]), data(this), request(this)
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

void Window::sendAction(Rawdata *rawdata)
{
    object[rawdata->file].polytope->action.put(*rawdata);
}

void Window::sendData(Rawdata *rawdata)
{
    object[rawdata->file].write->data.put(convert(*rawdata));
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
    completeCommand(query);
    if (isSuspend()) repeatCommand(pierce); else repeatCommand(redraw);
    std::string cmdstr; while (data.get(cmdstr)) processData(cmdstr);
    Command *command = 0; while (request.get(command)) consumeCommand(*command);}
    glfwTerminate();
}

void Window::wake()
{
	glfwPostEmptyEvent();
}

void Window::processData(std::string cmdstr)
{
    std::cout << cmdstr;
    std::string str;
    if (compare("--data",cmdstr,str)) {
    struct Rawdata rawdata; convert(str,rawdata); syncMatrix(&rawdata);}
    glfwPollEvents();
}

void Window::repeatCommand(Queue &queue)
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

void Window::completeCommand(Queue &queue)
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

void Window::consumeCommand(Command &command)
{
    Command *next = &command;
    while (next) {
    if (!next->finish) processCommand(*next);
    if (next->finish) finishCommand(*next);
    glfwPollEvents();
    if (next->finish) enque(query.first,query.last,next);    
    next = next->next;}
}

void Window::exchangeCommand(Queue &queue, Command *&command)
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
    for (Update *next = command.allocs; next; next = next->next) allocBuffer(*next);
    for (Update *next = command.writes; next; next = next->next) writeBuffer(*next);
    for (Render *next = command.renders; next; next = next->next) {
    Render &render = *next; Microcode &program = microcode[render.program];
    glUseProgram(program.handle); glBindVertexArray(object[render.file].vao[render.program][render.space]);
    for (Update *next = command.binds; next; next = next->next) bindBuffer(*next);
    if (command.feedback) {glEnable(GL_RASTERIZER_DISCARD); glBeginTransformFeedback(program.primitive);}
    if (render.count) glDrawElements(program.mode,render.count,GL_UNSIGNED_INT,reinterpret_cast<void*>(render.base));
    if (render.size) glDrawArrays(program.mode,render.base,render.size);
    if (command.feedback) {glEndTransformFeedback(); glDisable(GL_RASTERIZER_DISCARD);}
    for (Update *next = command.binds; next; next = next->next) unbindBuffer(*next);
    glBindVertexArray(0); glUseProgram(0);}
    if (command.feedback) glFlush(); else glfwSwapBuffers(window);
    command.finish = 1;
}

void Window::finishCommand(Command &command)
{
    for (Update *next = command.reads; next; next = next->next) {
    if (next->done == 0) {next->done = 1; readBuffer(*next);}
    if (next->done == 0) return;}
    if (command.pierce && pierce.last != pierce.loop) return;
    if (command.pierce) exchangeCommand(pierce,command.pierce);
    if (command.redraw) exchangeCommand(redraw,command.redraw);
    for (Response *next = command.response; next; next = next->next)
    object[next->file].polytope->response.put(command);
    for (Update *next = command.reads; next; next = next->next) next->done = 0;
    command.finish = 0;
}
