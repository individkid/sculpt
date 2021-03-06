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
#include "script.hpp"
#include "write.hpp"
#include "polytope.hpp"
extern "C" {
#include "arithmetic.h"
#include "callback.h"
}

void Window::connect(Script *ptr)
{
    req2script = &ptr->window2req;
    rsp2script = &ptr->window2rsp;
}

void Window::connect(int i, Write *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    req2write[i] = &ptr->window2req;
}

void Window::connect(Polytope *ptr)
{
    rsp2polytope = &ptr->window2rsp;
    req2polytope = &ptr->window2req;
}

void Window::init()
{
    if (rsp2script == 0) error("unconnected rsp2script",0,__FILE__,__LINE__);
    for (int i = 0; i < nfile; i++) if (req2write[i] == 0) error("unconnected req2write",i,__FILE__,__LINE__);
    if (req2polytope == 0) error("unconnected req2polytope",0,__FILE__,__LINE__);
    if (rsp2polytope == 0) error("unconnected rsp2polytope",0,__FILE__,__LINE__);
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
    processQueries(script2rsp);
    processDatas(polytope2rsp);
    processStates(write2rsp);
    processQueues(polytope);
    processCommands(polytope2req,polytope);
    processQueues(script);
    processCommands(script2req,script);
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
    processQueues(polytope);
    processCommands(polytope2req,polytope);
    processQueues(script);
    processCommands(script2req,script);
    glfwTerminate(); window = 0;
}

Window::Window(int n) : Thread(1),
    req2write(new Message<State>*[n]),
    script2rsp(this,"Window<-Query<-Script"),
    script2req(this,"Script->Command->Window"),
    write2rsp(this,"Window<-State<-Write"),
    polytope2req(this,"Polytope->Command->Window"),
    polytope2rsp(this,"Window<-Data<-Polytope"),
    window(0), object(new Object[n]), finish(0), nfile(n)
{
    Object obj = {0}; for (int i = 0; i < n; i++) object[i] = obj;
    Queues que = {0}; polytope = que; script = que;
}

extern Window *window;

static Power<char> chars(__FILE__,__LINE__);
static Pool<State> states(__FILE__,__LINE__);
static Power<float> floats(__FILE__,__LINE__);
static Pool<Data> datas(__FILE__,__LINE__);
static Pool<Query> queries(__FILE__,__LINE__);
static std::map<char,char*> hotkey;
static std::map<int,std::map<int,char*> > macro;

Window::~Window()
{
    if (window) error("done not called",0,__FILE__,__LINE__);
    processQueries(script2rsp);
    processDatas(polytope2rsp);
    processStates(write2rsp);
    for (std::map<char,char*>::iterator i = hotkey.begin(); i != hotkey.end(); i++)
        chars.put(strlen((*i).second)+1,(*i).second);
    hotkey.clear();
    for (std::map<int,std::map<int,char*> >::iterator i = macro.begin(); i != macro.end(); i++) {
        for (std::map<int,char*>::iterator j = (*i).second.begin(); j != (*i).second.end(); j++)
            chars.put(strlen((*j).second)+1,(*j).second);
        (*i).second.clear();}
    macro.clear();
}

extern "C" void sendMacro(int file, int plane)
{
    const char *script = "";
    if (macro.find(file) != macro.end() && macro[file].find(plane) != macro[file].end()) script = macro[file][plane];
    Query *query = queries.get(); query->script = chars.get(strlen(script)+1);
    query->file = file; query->plane = plane; query->given = IntGiv;
    strcpy(query->script,script);
    window->sendScript(query);
}

extern "C" void sendHotkey(char key)
{
    const char *script = "";
    if (hotkey.find(key) != hotkey.end()) script = hotkey[key];
    Query *query = queries.get(); query->script = chars.get(strlen(script)+1);
    query->key = key; query->given = CharGiv;
    strcpy(query->script,script);
    window->sendScript(query);
}

extern "C" void sendState(int file, enum Change change, float *matrix)
{
    State *state = states.get(); state->matrix = floats.get(16);
    state->file = file; state->change = change;
    for (int i = 0; i < 16; i++) state->matrix[i] = matrix[i];
    window->sendWrite(state);
}

extern "C" void sendSculpt(int file, int plane, enum Configure conf)
{
    Data *data = datas.get();
    data->file = file; data->plane = plane; data->conf = conf;
    window->sendPolytope(data);
}

extern "C" void sendRefine(int file, int plane, float *vector)
{
    Data *data = datas.get(); data->pierce = floats.get(3);
    data->file = file; data->plane = plane; data->conf = RefineConf;
    for (int i = 0; i < 3; i++) data->pierce[i] = vector[i];
    window->sendPolytope(data);
}

extern "C" void sendRelative(int file, int plane, enum TopologyMode topology, float *vector)
{
    Data *data = datas.get(); data->vector = floats.get(3);
    data->file = file; data->plane = plane; data->conf = AbsoluteConf; data->relative = topology;
    for (int i = 0; i < 3; i++) data->fixed[i] = vector[i];
    window->sendPolytope(data);
}

extern "C" void sendAbsolute(int file, int plane, enum TopologyMode topology)
{
    Data *data = datas.get();
    data->file = file; data->plane = plane; data->conf = RelativeConf; data->absolute = topology;
    window->sendPolytope(data);
}

extern "C" void sendFacet(int file, int plane, float *matrix)
{
    Data *data = datas.get(); data->matrix = floats.get(16);
    data->file = file; data->plane = plane; data->conf = ManipConf;
    for (int i = 0; i < 16; i++) data->matrix[i] = matrix[i];
    window->sendPolytope(data);
}

extern "C" void sendInvoke(int file, int plane, char press)
{
    Data *data = datas.get();
    data->file = file; data->plane = plane; data->conf = PressConf; data->press = press;
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

void Window::sendScript(Query *query)
{
    req2script->put(query);
}

void Window::sendWrite(State *state)
{
    req2write[state->file]->put(state);
}

void Window::sendPolytope(Data *data)
{
    req2polytope->put(data);
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
        if (command->source == MacroSrc) {
            std::map<int,std::map<int,char*> >::iterator i = macro.find(command->file);
            if (i != macro.end()) {
                std::map<int,char*>::iterator j = (*i).second.find(command->plane);
                if (j != (*i).second.end()) chars.put(strlen((*j).second)+1,(*j).second);}
            char *temp = chars.get(strlen(command->script)+1);
            strcpy(temp,command->script);
            macro[command->file][command->plane] = temp;}
        else if (command->source == HotkeySrc) {
            std::map<char,char*>::iterator i = hotkey.find(command->key);
            if (i != hotkey.end()) chars.put(strlen((*i).second)+1,(*i).second);
            char *temp = chars.get(strlen(command->script)+1);
            strcpy(temp,command->script);
            hotkey[command->key] = temp;}
        else changeState(command);
        if (command->source == PolytopeSrc) {
            if (command->next != 0) error("unsupported next",command->next,__FILE__,__LINE__);
            if (!command->finish) startCommand(*command);
            if (command->finish) finishCommand(*command,queues);
            if (command->finish) {
                if (&message == &polytope2req) enque(polytope.query.first,polytope.query.last,command);}}
        else if (&message == &script2req) {
            rsp2script->put(command);}
        else if (&message == &polytope2req) {
            rsp2polytope->put(command);}
        else error("invalid message",0,__FILE__,__LINE__);
    }
}

void Window::processQueries(Message<Query> &message)
{
    Query *query; while (message.get(query)) {
        chars.put(strlen(query->script)+1,query->script);
        queries.put(query);}
}

void Window::processDatas(Message<Data> &message)
{
    Data *data; while (message.get(data)) {
        switch (data->conf) {
        case (RelativeConf): floats.put(3,data->fixed); break;
        case (AbsoluteConf): break;
        case (RefineConf): floats.put(3,data->pierce); break;
        case (ManipConf): floats.put(16,data->matrix); break;
        case (PressConf): break;
        case (AdditiveConf): break;
        case (SubtractiveConf): break;
        default: error("invalid conf",data->conf,__FILE__,__LINE__);}
        datas.put(data);}
}

void Window::processStates(Message<State> &message)
{
    State *state; while (message.get(state)) {
        switch (state->change) {
        case (SculptChange): break;
        case (GlobalChange): floats.put(16,state->matrix); break;
        case (MatrixChange): floats.put(16,state->matrix); break;
        default: error("invalid change",state->change,__FILE__,__LINE__);}
        states.put(state);}
}
