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

#include "window.hpp"
#include "write.hpp"
#include "read.hpp"

extern "C" void displayError(int error, const char *description)
{
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

int testGoon = 1;
extern "C" void displayKey(GLFWwindow* ptr, int key, int scancode, int action, int mods)
{
    if (action == 1) std::cout << "GLFW key " << key << " " << scancode << " " << action << " " << mods << std::endl;
    if (key == 256 && action == 1 && testGoon == 1) testGoon = 2;
    if (key == 257 && action == 1 && testGoon == 2) testGoon = 0;
}

void Window::initProgram(Program program)
{
    switch (program) {
    case (Diplane): break;
    case (Dipoint): initDipoint(); break;
    case (Coplane): break;
    case (Copoint): break;
    case (Adplane): break;
    case (Adpoint): break;
    case (Perplane): break;
    case (Perpoint): break;
    case (Replane): break;
    case (Repoint): break;
    case (Explane): break;
    case (Expoint): break;
    default: error("invalid program",program,__FILE__,__LINE__);}
}

void Window::initDipoint()
{
	const char *vertex = "\
    layout (location = 0) in vec3 point;\n\
    layout (location = 1) in vec3 normal[3];\n\
    layout (location = 4) in vec2 coordinate[3];\n\
    layout (location = 7) in uvec4 weight[3];\n\
    layout (location = 10) in vec3 color[3];\n\
    layout (location = 13) in uvec2 tag[3];\n\
    out vec4 vertexColor;\n\
    void main()\n\
    {\n\
    vec3 vector = (affine*vec4(point,1.0)).xyz;\n\
    vector.x = vector.x/(vector.z*slope+1.0);\n\
    vector.y = vector.y/(vector.z*slope*aspect+aspect);\n\
    vector.z = vector.z/cutoff;\n\
    gl_Position = vec4(vector,1.0);\n\
    if (taggraph == tag[0].x) vertexColor.xyz = color[0];\n\
    else if (taggraph == tag[1].x) vertexColor.xyz = color[1];\n\
    else if (taggraph == tag[2].x) vertexColor.xyz = color[2];\n\
    else vertexColor.xyz = vec3(0.0,0.0,0.0);\n\
    vertexColor.w = 1.0;\n\
    }\n";
    const char *fragment = "\
    out vec4 FragColor;\n\
    in vec4 vertexColor;\n\
    void main()\n\
    {\n\
    FragColor = vertexColor;\n\
    }\n";
    initConfigure(vertex,0,fragment,0,0,configure[Dipoint]);
}

void Window::initConfigure(const char *vertex, const char *geometry, const char *fragment, int count, const char **feedback, Configure &program)
{
    program.handle = glCreateProgram();
    MYuint vertexIdent; initShader(GL_VERTEX_SHADER, vertex, vertexIdent); glAttachShader(program.handle, vertexIdent);
    MYuint geometryIdent; if (geometry) {initShader(GL_GEOMETRY_SHADER, geometry, geometryIdent); glAttachShader(program.handle, geometryIdent);}
    MYuint fragmentIdent; initShader(GL_FRAGMENT_SHADER, fragment, fragmentIdent); glAttachShader(program.handle, fragmentIdent);
    if (count) glTransformFeedbackVaryings(program.handle,count,feedback,GL_SEPARATE_ATTRIBS);
    glLinkProgram(program.handle);
    GLint status = GL_FALSE; glGetProgramiv(program.handle, GL_LINK_STATUS, &status);
    int length; glGetProgramiv(program.handle, GL_INFO_LOG_LENGTH, &length); if (length > 0) {
    GLchar info[length+1]; glGetProgramInfoLog(program.handle, length, NULL, info); std::cerr << info << std::endl; exit(-1);}
    glDetachShader(program.handle, vertexIdent); glDeleteShader(vertexIdent);
    if (geometry) {glDetachShader(program.handle, geometryIdent); glDeleteShader(geometryIdent);}
    glDetachShader(program.handle, fragmentIdent); glDeleteShader(fragmentIdent);
    glUniformBlockBinding(program.handle,glGetUniformBlockIndex(program.handle,"Uniform"),0);
    glUniformBlockBinding(program.handle,glGetUniformBlockIndex(program.handle,"Global"),1);
	program.mode = GL_TRIANGLES;
	program.primitive = GL_POINTS;
}

void Window::initShader(MYenum type, const char *source, MYuint &ident)
{
    const char *code[2] = {
    "#version 330 core\n\
    layout(std140) uniform Uniform {\n\
    mat4 perfile;\n\
    };\n\
    layout(std140) uniform Global {\n\
    mat4 affine;\n\
    mat4 perplane;\n\
    uint tagplane;\n\
    uint taggraph;\n\
    float cutoff;\n\
    float slope;\n\
    float aspect;\n\
    };",
    source};
    ident = glCreateShader(type); glShaderSource(ident, 2, code, NULL); glCompileShader(ident);
    GLint status = GL_FALSE; glGetShaderiv(ident, GL_COMPILE_STATUS, &status);
    int length; glGetShaderiv(ident, GL_INFO_LOG_LENGTH, &length); if (length > 0) {
    GLchar info[length+1]; glGetShaderInfoLog(ident, length, NULL, info); std::cerr << info << std::endl; exit(-1);}
}

void Window::initFile(File &file)
{
	for (Buffer b = (Buffer)0; b < Buffers; b = (Buffer)((int)b+1))
	initHandle(b,(&file==this->file),file.handle[b]);
	for (Program p = (Program)0; p < Programs; p = (Program)((int)p+1))
	for (Space s = (Space)0; s < Spaces; s = (Space)((int)s+1))
	glGenVertexArrays(1, &file.vao[p][s]);
	for (Buffer b = (Buffer)0; b < Buffers; b = (Buffer)((int)b+1))
	for (Program p = (Program)0; p < Programs; p = (Program)((int)p+1))
	for (Space s = (Space)0; s < Spaces; s = (Space)((int)s+1))
	initVao(b,p,s,file.vao[p][s],file.handle[b].handle);
}

void Window::initHandle(enum Buffer buffer, int first, Handle &handle)
{
	switch (buffer) {
	case (Texture0): handle.unit = GL_TEXTURE0; break;
    case (Texture1): handle.unit = GL_TEXTURE1; break;
    case (Uniform): case (Global): if (!first) handle.handle = 0; break;
	default: glGenBuffers(1,&handle.handle); break;}
    switch (buffer) {
    case (Face): case (Frame): case (Coface): case (Coframe): case (Incidence): case (Block): handle.target = GL_ELEMENT_ARRAY_BUFFER; break;
    case (Construct): case (Dimension): case (Vertex): case (Vector): case (Pierce): case (Side): handle.target = GL_TRANSFORM_FEEDBACK_BUFFER; break;
    case (Uniform): case (Global): handle.target = GL_UNIFORM_BUFFER; break;
    case (Texture0): case (Texture1): handle.target = GL_TEXTURE_2D; break;
    default: handle.target = GL_ARRAY_BUFFER; break;}
    switch (buffer) {
    case (Construct): case (Dimension): case (Vertex): case (Vector): case (Pierce): case (Side): handle.usage = GL_STATIC_READ; break;
    default: handle.usage = GL_STATIC_DRAW; break;}
    switch (buffer) {
    case (Dimension): case (Global): handle.index = 1; break;
    default: handle.index = 0;}
}

void Window::initVao(enum Buffer buffer, enum Program program, enum Space space, MYuint vao, MYuint handle)
{
	glBindVertexArray(vao);
	switch (program) {
    case (Diplane): break;
	case (Dipoint): switch (buffer) {
	case (Frame): if (space == Small) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle); break;
	case (Coframe): if (space == Large) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle); break;
	case (Point): initVao3f(0,handle); break;
	case (Normal): for (int i = 1; i < 4; i++) initVao3f(i,handle); break;
	case (Coordinate): for (int i = 4; i < 7; i++) initVao2f(i,handle); break;
	case (Weight): for (int i = 7; i < 10; i++) initVao4u(i,handle); break;
	case (Color): for (int i = 10; i < 13; i++) initVao3f(i,handle); break;
	case (Tag): for (int i = 13; i < 16; i++) initVao2u(i,handle); break;
	default: break;}
    case (Coplane): break;
    case (Copoint): break;
    case (Adplane): break;
    case (Adpoint): break;
    case (Perplane): break;
    case (Perpoint): break;
    case (Replane): break;
    case (Repoint): break;
    case (Explane): break;
    case (Expoint): break;
	default: error("invalid program",program,__FILE__,__LINE__);}
	glBindVertexArray(0);
}

void Window::initVao3f(MYuint index, MYuint handle)
{
	glBindBuffer(GL_ARRAY_BUFFER,handle);
	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(index);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void Window::initVao2f(MYuint index, MYuint handle)
{
	glBindBuffer(GL_ARRAY_BUFFER,handle);
	glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(index);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void Window::initVao4u(MYuint index, MYuint handle)
{
	glBindBuffer(GL_ARRAY_BUFFER,handle);
	glVertexAttribIPointer(index, 4, GL_UNSIGNED_INT, 4 * sizeof(unsigned), (void*)0);
	glEnableVertexAttribArray(index);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void Window::initVao2u(MYuint index, MYuint handle)
{
	glBindBuffer(GL_ARRAY_BUFFER,handle);
	glVertexAttribIPointer(index, 2, GL_UNSIGNED_INT, 2 * sizeof(unsigned), (void*)0);
	glEnableVertexAttribArray(index);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void Window::allocBuffer(Update &update)
{
    Handle &buffer = file[update.file].handle[update.buffer];
    if (buffer.target == GL_TEXTURE_2D) {allocTexture2d(update); return;}
    glBindBuffer(buffer.target,buffer.handle);
    glBufferData(buffer.target,update.size,NULL,buffer.usage);
    glBindBuffer(buffer.target,0);
}

void Window::writeBuffer(Update &update)
{
    Handle &buffer = file[update.file].handle[update.buffer];
    if (buffer.target == GL_TEXTURE_2D) {writeTexture2d(update); return;}
    glBindBuffer(buffer.target,buffer.handle);
    glBufferSubData(buffer.target,update.offset,update.size,update.data);
    glBindBuffer(buffer.target,0);
}

void Window::bindBuffer(Update &update)
{
    Handle &buffer = file[update.file].handle[update.buffer];
    if (buffer.target == GL_TEXTURE_2D) {bindTexture2d(update); return;}
    glBindBufferBase(buffer.target,buffer.index,buffer.handle);
}

void Window::unbindBuffer(Update &update)
{
    Handle &buffer = file[update.file].handle[update.buffer];
    if (buffer.target == GL_TEXTURE_2D) {unbindTexture2d(); return;}
    glBindBufferBase(buffer.target,buffer.index,0);
}

void Window::readBuffer(Update &update)
{
    Handle &buffer = file[update.file].handle[update.buffer];
    if (buffer.target == GL_TEXTURE_2D) return;
    glBindBuffer(buffer.target,buffer.handle);
    glGetBufferSubData(buffer.target,update.offset,update.size,update.data);
    glBindBuffer(buffer.target,0);
}

void Window::allocTexture2d(Update &update)
{
    Handle &buffer = file[update.file].handle[update.buffer];
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
    Handle &buffer = file[update.file].handle[update.buffer];
    glActiveTexture(buffer.handle);
    glBindTexture(GL_TEXTURE_2D,update.handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, update.width, update.height, 0, GL_RGB, GL_UNSIGNED_BYTE, update.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,0);
}

void Window::bindTexture2d(Update &update)
{
    Handle &buffer = file[update.file].handle[update.buffer];
    glActiveTexture(buffer.handle);
    glBindTexture(GL_TEXTURE_2D,update.handle);
}

void Window::unbindTexture2d()
{
    glBindTexture(GL_TEXTURE_2D,0);
}

Window::Window(int n) : Thread(1), write(new Write *[n]), polytope(new Polytope *[n]), read(new Read *[n]), window(0), nfile(n), file(new File[n]), mode(this), data(this), request(this)
{
}


void Window::call()
{
    if (sizeof(GLenum) != sizeof(MYenum)) error("sizeof enum",sizeof(GLenum),__FILE__,__LINE__);
    if (sizeof(GLuint) != sizeof(MYuint)) error("sizeof uint",sizeof(GLuint),__FILE__,__LINE__);
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
    for (Program p = (Program)0; p < Programs; p = (Program)((int)p+1)) initProgram(p);
    for (int f = 0; f < nfile; f++) initFile(file[f]);
    while (testGoon) {Command command;
    if (request.get(command)) {
    for (Next<Update> *next = command.allocs; next; next = next->next) allocBuffer(next->box);
    for (Next<Update> *next = command.writes; next; next = next->next) writeBuffer(next->box);
    for (Next<Render> *next = command.renders; next; next = next->next) {
    Render render = next->box; Configure program = configure[render.program];
    glUseProgram(program.handle);
    glBindVertexArray(file[render.file].vao[render.program][render.space]);
    for (Next<Update> *next = command.binds; next; next = next->next) bindBuffer(next->box);
    if (command.feedback) {
        glEnable(GL_RASTERIZER_DISCARD);
        glBeginTransformFeedback(program.primitive);}
	if (render.count) glDrawElements(program.mode,render.count,GL_UNSIGNED_INT,reinterpret_cast<void*>(render.base));
	if (render.size) glDrawArrays(program.mode,render.base,render.size);
	if (command.feedback) {
        glEndTransformFeedback();
        glDisable(GL_RASTERIZER_DISCARD);}
    for (Next<Update> *next = command.binds; next; next = next->next) unbindBuffer(next->box);
	glBindVertexArray(0);
	glUseProgram(0);}
	if (command.feedback) glFlush();
	else glfwSwapBuffers(window);
    for (Next<Update> *next = command.reads; next; next = next->next) readBuffer(next->box);
	if (command.response) command.response->put(command);}
    glfwWaitEvents();}
    glfwTerminate();
}

void Window::wake()
{
	glfwPostEmptyEvent();
}

