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

#include "window.hpp"

extern "C" {

void displayError(int error, const char *description)
{
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

int testGoon = 1;
void displayKey(GLFWwindow* ptr, int key, int scancode, int action, int mods)
{
    if (action == 1) std::cout << "GLFW key " << key << " " << scancode << " " << action << " " << mods << std::endl;
    if (key == 256 && action == 1 && testGoon == 1) testGoon = 2;
    if (key == 257 && action == 1 && testGoon == 2) testGoon = 0;
}

}

GLuint Window::compileShader(GLenum type, const char *source)
{
	const char *code[2] = {
	"#version 330 core\n\
	layout(std140) uniform Uniform {\n\
    uniform mat4 affine;\n\
    uniform float cutoff;\n\
    uniform float slope;\n\
    uniform float aspect;\n\
	};",
	source};
    GLuint ident = glCreateShader(type); glShaderSource(ident, 2, code, NULL); glCompileShader(ident);
    GLint status = GL_FALSE; glGetShaderiv(ident, GL_COMPILE_STATUS, &status);
    int length; glGetShaderiv(ident, GL_INFO_LOG_LENGTH, &length); if (length > 0) {
    GLchar info[length+1]; glGetShaderInfoLog(ident, length, NULL, info); std::cerr << info << std::endl; exit(-1);}
    return ident;
}

void Window::initDipoint()
{
	Configure *program = &configure[Dipoint];
	const char *vertex = "\
    layout (location = 0) in vec3 point;\n\
    out data {\n\
    vec3 point;\n\
    } od;\n\
    void main()\n\
    {\n\
    od.point = (affine*vec4(point,1.0)).xyz;\n\
    }\n";
    const char *geometry = "\
    layout (triangles) in;\n\
    layout (triangle_strip, max_vertices = 3) out;\n\
    in data {\n\
    vec3 point;\n\
    } id[3];\n\
    out vec3 normal;\n\
    void main()\n\
    {\n\
    for (int i = 0; i < 3; i++) {\n\
    vec3 vector = id[i].point;\n\
    vector.x = vector.x/(vector.z*slope+1.0);\n\
    vector.y = vector.y/(vector.z*slope*aspect+aspect);\n\
    vector.z = vector.z/cutoff;\n\
    gl_Position = vec4(vector,1.0);\n\
    normal = vec3(1.0,1.0,1.0);\n\
    normal[i] = 0.0;\n\
    EmitVertex();}\n\
    EndPrimitive();\n\
    }\n";
    const char *fragment = "\
    in vec3 normal;\n\
    out vec4 result;\n\
    void main()\n\
    {\n\
    result = vec4(normal, 1.0f);\n\
    }\n";
    program->handle = glCreateProgram();
    GLuint vertexIdent = compileShader(GL_VERTEX_SHADER, vertex); glAttachShader(program->handle, vertexIdent);
    // TODO check for null geometry shader (as it should be for dipoint)
    GLuint geometryIdent = compileShader(GL_GEOMETRY_SHADER, geometry); glAttachShader(program->handle, geometryIdent);
    GLuint fragmentIdent = compileShader(GL_FRAGMENT_SHADER, fragment); glAttachShader(program->handle, fragmentIdent);
    // glTransformFeedbackVaryings(program->handle,array-of-names-count,array-of-names,GL_SEPARATE_ATTRIBS);
    glLinkProgram(program->handle);
    GLint status = GL_FALSE; glGetProgramiv(program->handle, GL_LINK_STATUS, &status);
    int length; glGetProgramiv(program->handle, GL_INFO_LOG_LENGTH, &length); if (length > 0) {
    GLchar info[length+1]; glGetProgramInfoLog(program->handle, length, NULL, info); std::cerr << info << std::endl; exit(-1);}
    glDetachShader(program->handle, vertexIdent); glDeleteShader(vertexIdent);
    glDetachShader(program->handle, geometryIdent); glDeleteShader(geometryIdent);
    glDetachShader(program->handle, fragmentIdent); glDeleteShader(fragmentIdent);
    glUniformBlockBinding(program->handle,glGetUniformBlockIndex(program->handle,"Uniform"),0);
	program->mode = GL_TRIANGLES;
	program->primitive = GL_POINTS;
}

void Window::initFile(File *file)
{
	for (Buffer b = (Buffer)0; b < Buffers; b = (Buffer)((int)b+1))
	initHandle(b,&file->handle[b]);
	for (Program p = (Program)0; p < Programs; p = (Program)((int)p+1))
	for (Space s = (Space)0; s < Spaces; s = (Space)((int)s+1))
	glGenVertexArrays(1, &file->vao[p][s]);
	for (Buffer b = (Buffer)0; b < Buffers; b = (Buffer)((int)b+1))
	for (Program p = (Program)0; p < Programs; p = (Program)((int)p+1))
	for (Space s = (Space)0; s < Spaces; s = (Space)((int)s+1))
	initVao(b,p,s,file->vao[p][s],file->handle[b].handle);
}

void Window::initHandle(enum Buffer buffer, Handle *handle)
{
	switch (buffer) {
	case (Texture): glGenTextures(1,&handle->handle); break;
	default: glGenBuffers(1,&handle->handle); break;}
    switch (buffer) {
    case (Face): case (Frame): case (Coface): case (Coframe): case (Incidence): case (Block): handle->target = GL_ELEMENT_ARRAY_BUFFER; break;
    case (Construct): case (Dimension): case (Vertex): case (Vector): case (Pierce): case (Side): handle->target = GL_TRANSFORM_FEEDBACK_BUFFER; break;
    case (Uniform): case (Global): handle->target = GL_UNIFORM_BUFFER; break;
    case (Texture): handle->target = GL_TEXTURE_2D; break;
    default: handle->target = GL_ARRAY_BUFFER; break;}
    switch (buffer) {
    case (Construct): case (Dimension): case (Vertex): case (Vector): case (Pierce): case (Side): handle->usage = GL_STATIC_READ; break;
    default: handle->usage = GL_STATIC_DRAW; break;}
    switch (buffer) {
    case (Dimension): case (Global): handle->index = 1; break;
    default: handle->index = 0;}
}

void Window::initVao(enum Buffer buffer, enum Program program, enum Space space, GLuint vao, GLuint handle)
{
	glBindVertexArray(vao);
	switch (program) {
	case (Dipoint): switch (buffer) {
	case (Frame): if (space == Small) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle); break;
	case (Coframe): if (space == Large) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle); break;
	case (Point): initVao3f(0,handle); break;
	case (Normal): for (int i = 1; i < 4; i++) initVao3f(i,handle); break;
	case (Coordinate): for (int i = 4; i < 7; i++) initVao2f(i,handle); break;
	case (Weight): for (int i = 7; i < 10; i++) initVao4u(i,handle); break;
	case (Color): for (int i = 10; i < 13; i++) initVao3f(i,handle); break;
	case (Tag): for (int i = 13; i < 16; i++) initVao2b(i,handle); break;
	default: break;}
	default: std::cerr << "invalid program" << std::endl; exit(-1);}
	glBindVertexArray(0);
}

void Window::initVao3f(GLuint index, GLuint handle)
{
	glBindBuffer(GL_ARRAY_BUFFER,handle);
	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(index);
}

void Window::initVao2f(GLuint index, GLuint handle)
{
	glBindBuffer(GL_ARRAY_BUFFER,handle);
	glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(index);
}

void Window::initVao4u(GLuint index, GLuint handle)
{
	glBindBuffer(GL_ARRAY_BUFFER,handle);
	glVertexAttribIPointer(index, 4, GL_UNSIGNED_INT, 4 * sizeof(unsigned), (void*)0);
	glEnableVertexAttribArray(index);
}

void Window::initVao2b(GLuint index, GLuint handle)
{
	glBindBuffer(GL_ARRAY_BUFFER,handle);
	glVertexAttribIPointer(index, 2, GL_UNSIGNED_BYTE, 2 * sizeof(char), (void*)0);
	glEnableVertexAttribArray(index);
}

void Window::allocBuffer(Update update)
{
    Handle buffer = file[update.file].handle[update.buffer];
    if (buffer.target == GL_TEXTURE_2D) {
    glActiveTexture(indexTexture(update.unit));
    glBindTexture(GL_TEXTURE_2D,buffer.handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return;}
    glBindBuffer(buffer.target,buffer.handle);
    glBufferData(buffer.target,update.size,NULL,buffer.usage);
}

void Window::writeBuffer(Update update)
{
    Handle buffer = file[update.file].handle[update.buffer];
    if (buffer.target == GL_TEXTURE_2D) {
    glActiveTexture(indexTexture(update.unit));
    glBindTexture(GL_TEXTURE_2D,buffer.handle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, update.width, update.height, 0, GL_RGB, GL_UNSIGNED_BYTE, update.data);
	glGenerateMipmap(GL_TEXTURE_2D);
    return;}
    glBindBuffer(buffer.target,buffer.handle);
    glBufferSubData(buffer.target,update.offset,update.size,update.data);
}

void Window::bindBuffer(Update update)
{
    Handle buffer = file[update.file].handle[update.buffer];
    if (buffer.target == GL_TEXTURE_2D) {
    glActiveTexture(indexTexture(update.unit));
    glBindTexture(GL_TEXTURE_2D,buffer.handle);
    return;}
    glBindBufferBase(buffer.target,buffer.index,buffer.handle);
}

void Window::unbindBuffer(Update update)
{
    Handle buffer = file[update.file].handle[update.buffer];
    if (buffer.target == GL_TEXTURE_2D) {
    glBindTexture(GL_TEXTURE_2D,0);
    return;}
    glBindBufferBase(buffer.target,buffer.index,0);
}

void Window::readBuffer(Update update)
{
    Handle buffer = file[update.file].handle[update.buffer];
    if (buffer.target == GL_TEXTURE_2D) return;
    glBindBuffer(buffer.target,buffer.handle);
    glGetBufferSubData(buffer.target,update.offset,update.size,update.data);
}

GLenum Window::indexTexture(int unit)
{
	switch (unit) {
	case (1): return GL_TEXTURE1;
	default: break;}
	return GL_TEXTURE0;
}

void Window::run()
{
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
    initDipoint();
    for (int f = 0; f < nfile; f++) initFile(file+f);
    while (testGoon) {Command command;
    if (request.get(command)) {
    for (Next<Update> *next = command.allocs; next; next = next->next) allocBuffer(next->box);
    for (Next<Update> *next = command.writes; next; next = next->next) writeBuffer(next->box);
    for (Next<Render> *next = command.renders; next; next = next->next) {
    Render render = next->box; Configure program = configure[render.program];
    glUseProgram(program.handle);
    glBindVertexArray(file[render.file].vao[render.program][render.space]);
    if (command.binds) {
        glEnable(GL_RASTERIZER_DISCARD);
    for (Next<Update> *next = command.binds; next; next = next->next) bindBuffer(next->box);
        glBeginTransformFeedback(program.primitive);}
	if (render.count) glDrawElements(program.mode,render.count,GL_UNSIGNED_INT,reinterpret_cast<void*>(render.base));
	if (render.size) glDrawArrays(program.mode,render.base,render.size);
	if (command.binds) {
        glEndTransformFeedback();
    for (Next<Update> *next = command.binds; next; next = next->next) unbindBuffer(next->box);
        glDisable(GL_RASTERIZER_DISCARD);}
	glBindVertexArray(0);
	glUseProgram(0);}
	if (command.binds) glFlush();
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

