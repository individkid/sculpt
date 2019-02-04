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

Handle Window::handle[Buffers] = {0};
Configure Window::configure[Programs] = {0};
int Window::once = 0;

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

GLuint Window::compileProgram(const char *vertex, const char *geometry, const char *fragment)
{
    GLuint ident = glCreateProgram();
    GLuint vertexIdent = compileShader(GL_VERTEX_SHADER, vertex); glAttachShader(ident, vertexIdent);
    GLuint geometryIdent = compileShader(GL_GEOMETRY_SHADER, geometry); glAttachShader(ident, geometryIdent);
    GLuint fragmentIdent = compileShader(GL_FRAGMENT_SHADER, fragment); glAttachShader(ident, fragmentIdent);
    glLinkProgram(ident);
    GLint status = GL_FALSE; glGetProgramiv(ident, GL_LINK_STATUS, &status);
    int length; glGetProgramiv(ident, GL_INFO_LOG_LENGTH, &length); if (length > 0) {
    GLchar info[length+1]; glGetProgramInfoLog(ident, length, NULL, info); std::cerr << info << std::endl; exit(-1);}
    glDetachShader(ident, vertexIdent); glDeleteShader(vertexIdent);
    glDeleteShader(geometryIdent); glDetachShader(ident, geometryIdent);
    glDetachShader(ident, fragmentIdent); glDeleteShader(fragmentIdent);
    return ident;
}

void Window::configureDipoint()
{
	Configure *program = &configure[Dipoint];
	program->vertex = "\
    layout (location = 0) in vec3 point;\n\
    out data {\n\
    vec3 point;\n\
    } od;\n\
    void main()\n\
    {\n\
    od.point = (affine*vec4(point,1.0)).xyz;\n\
    }\n";
    program->geometry = "\
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
    program->fragment = "\
    in vec3 normal;\n\
    out vec4 result;\n\
    void main()\n\
    {\n\
    result = vec4(normal, 1.0f);\n\
    }\n";
    program->handle = compileProgram(program->vertex,program->geometry,program->fragment);
	glGenVertexArrays(1, &program->vao);
	glBindVertexArray(program->vao);
	glBindBuffer(GL_ARRAY_BUFFER,handle[Point].handle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[Frame].handle);
	glBindVertexArray(0);
	program->mode = GL_TRIANGLES;
	program->primitive = GL_POINTS;
	program->feedback = 0;
    glUniformBlockBinding(program->handle,glGetUniformBlockIndex(program->handle,"Uniform"),0);
}

void Window::initOnce()
{
    for (Buffer b = (Buffer)0; b < Buffers; b = (Buffer)((int)b+1)) glGenBuffers(1,&handle[b].handle);
    for (Buffer b = (Buffer)0; b < Buffers; b = (Buffer)((int)b+1)) switch (b) {
    case (Face): case (Frame): case (Coplane): case (Incidence): handle[b].target = GL_ELEMENT_ARRAY_BUFFER; break;
    case (Construct): case (Vertex): case (Dimension): handle[b].target = GL_TRANSFORM_FEEDBACK_BUFFER; break;
    case (Texture): handle[b].target = GL_TEXTURE_BUFFER; break;
    case (Uniform): handle[b].target = GL_UNIFORM_BUFFER; break;
    default: handle[b].target = GL_ARRAY_BUFFER; break;}
    for (Buffer b = (Buffer)0; b < Buffers; b = (Buffer)((int)b+1)) switch (b) {
    case (Construct): case (Vertex): glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,handle[b].handle); break;
    case (Dimension): glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,1,handle[b].handle); break;
    case (Uniform): glBindBufferBase(GL_UNIFORM_BUFFER,0,handle[b].handle); break;
    default: break;}
    configureDipoint();
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
    if (!once) {once = 1; initOnce();}
    while (testGoon) {Command command;
    if (request && request->get(command)) {
    for (Next<Subcmd> *next = command.allocs; next; next = next->next) {
    Subcmd subcmd = next->box; Handle buffer = handle[subcmd.buffer];
    glBindBuffer(buffer.target,buffer.handle);
    glBufferData(buffer.target,subcmd.size,NULL,GL_STATIC_DRAW);}
    for (Next<Subcmd> *next = command.writes; next; next = next->next) {
    Subcmd subcmd = next->box; Handle buffer = handle[subcmd.buffer];
    glBindBuffer(buffer.target,buffer.handle);
    glBufferSubData(buffer.target,subcmd.offset,subcmd.size,subcmd.data);}
    for (Next<Subcmd> *next = command.reads; next; next = next->next) {
    Subcmd subcmd = next->box; Handle buffer = handle[subcmd.buffer];
    glBindBuffer(buffer.target,buffer.handle);
    glGetBufferSubData(buffer.target,subcmd.offset,subcmd.size,subcmd.data);}
    Configure program = configure[command.program];
    glUseProgram(program.handle);
    glBindVertexArray(program.vao);
    if (program.feedback) {
        glEnable(GL_RASTERIZER_DISCARD);
        glBeginTransformFeedback(program.primitive);}
	if (command.count) glDrawElements(program.mode,command.count,GL_UNSIGNED_INT,NULL);
	if (command.size) glDrawArrays(program.mode,0,command.size);
	if (program.feedback) {
        glEndTransformFeedback();
        glDisable(GL_RASTERIZER_DISCARD);}
	glBindVertexArray(0);
	glUseProgram(0);
	if (program.feedback) glFlush();
	else glfwSwapBuffers(window);
	if (response) response->put(command);}
    glfwWaitEvents();}
    glfwTerminate();
}

void Window::wake()
{
	glfwPostEmptyEvent();
}

