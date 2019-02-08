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

Configure Window::configure[Programs] = {0};

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

void Window::configureDipoint()
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
    // glTransformFeedbackVaryings(program->handle,feedback,array-of-names,GL_SEPARATE_ATTRIBS);
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
	program->feedback = 0;
}

void Window::initVao(File *file, enum Program program, enum Buffer buffer)
{
	glGenVertexArrays(1, &file->vao[program][Small]);
	glBindVertexArray(file->vao[program][Small]);
	glBindBuffer(GL_ARRAY_BUFFER,file->handle[buffer].handle);
	// TODO enable other input buffers for dipoint shaders
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, file->handle[buffer].handle);
	glBindVertexArray(0);
}

void Window::initBuffer(Handle *handle)
{
    for (Buffer b = (Buffer)0; b < Buffers; b = (Buffer)((int)b+1)) glGenBuffers(1,&handle[b].handle);
    for (Buffer b = (Buffer)0; b < Buffers; b = (Buffer)((int)b+1)) switch (b) {
    case (Face): case (Frame): case (Incidence): case (Block): handle[b].target = GL_ELEMENT_ARRAY_BUFFER; break;
    case (Construct): case (Dimension): case (Vertex): case (Vector): case (Pierce): case (Side): handle[b].target = GL_TRANSFORM_FEEDBACK_BUFFER; break;
    case (Uniform): handle[b].target = GL_UNIFORM_BUFFER; break;
    case (Texture): handle[b].target = GL_TEXTURE_BUFFER; break;
    default: handle[b].target = GL_ARRAY_BUFFER; break;}
    for (Buffer b = (Buffer)0; b < Buffers; b = (Buffer)((int)b+1)) switch (b) {
    case (Construct): case (Dimension): case (Vertex): case (Vector): case (Pierce): case (Side): handle[b].usage = GL_STATIC_READ; break;
    default: handle[b].usage = GL_STATIC_DRAW; break;}
    for (Buffer b = (Buffer)0; b < Buffers; b = (Buffer)((int)b+1)) switch (b) {
    case (Construct): case (Vertex): case (Vector): case (Pierce): case (Side): glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,handle[b].handle); break;
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
    // initialize programs and per file per argc
    while (testGoon) {Command command;
    if (request && request->get(command)) {
    // TODO update textures and uniforms differently
    for (Next<Update> *next = command.allocs; next; next = next->next) {
    Update update = next->box; Handle buffer = file[update.file].handle[update.buffer];
    glBindBuffer(buffer.target,buffer.handle);
    glBufferData(buffer.target,update.size,NULL,buffer.usage);}
    for (Next<Update> *next = command.writes; next; next = next->next) {
    Update update = next->box; Handle buffer = file[update.file].handle[update.buffer];
    glBindBuffer(buffer.target,buffer.handle);
    glBufferSubData(buffer.target,update.offset,update.size,update.data);}
    for (Next<Update> *next = command.reads; next; next = next->next) {
    Update update = next->box; Handle buffer = file[update.file].handle[update.buffer];
    glBindBuffer(buffer.target,buffer.handle);
    glGetBufferSubData(buffer.target,update.offset,update.size,update.data);}
    int feedback = 0; int display = 0;
    for (Next<Render> *next = command.renders; next; next = next->next) {
    Render render = next->box; Configure program = configure[render.program];
    if (program.feedback) feedback = 1; else display = 1;
    glUseProgram(program.handle);
    glBindVertexArray(file[render.file].vao[render.program][render.space]);
    if (program.feedback) {
        glEnable(GL_RASTERIZER_DISCARD);
        glBeginTransformFeedback(program.primitive);}
	if (render.count) glDrawElements(program.mode,render.count,GL_UNSIGNED_INT,reinterpret_cast<void*>(render.base));
	if (render.size) glDrawArrays(program.mode,render.base,render.size);
	if (program.feedback) {
        glEndTransformFeedback();
        glDisable(GL_RASTERIZER_DISCARD);}
	glBindVertexArray(0);
	glUseProgram(0);}
	if (feedback) glFlush();
	if (display) glfwSwapBuffers(window);
	if (command.response) command.response->put(command);}
    glfwWaitEvents();}
    glfwTerminate();
}

void Window::wake()
{
	glfwPostEmptyEvent();
}

