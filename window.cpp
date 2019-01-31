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

enum Program {
    Diplane, Dipoint,
    Coplane, Copoint,
    Adplane, Adpoint,
    Perplane, Perpoint,
    Replane, Repoint,
    Programs};
const char *vertexCode[Programs] = {
    0, // Diplane
    "\
    uniform mat4 affine;\n\
    layout (location = 0) in vec3 point;\n\
    out data {\n\
    vec3 point;\n\
    } od;\n\
    void main()\n\
    {\n\
    od.point = (affine*vec4(point,1.0)).xyz;\n\
    }\n", // Dipoint
    0, // Coplane
    0, // Copoint
    0, // Adplane
    0, // Adpoint
    0, // Perplane
    0, // Perpoint
    0, // Replane
    0}; // Repoint
const char *geometryCode[Programs] = {
    0, // Diplane
    "\
    uniform float cutoff;\n\
    uniform float slope;\n\
    uniform float aspect;\n\
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
    }\n", // Dipoint
    0, // Coplane
    0, // Copoint
    0, // Adplane
    0, // Adpoint
    0, // Perplane
    0, // Perpoint
    0, // Replane
    0}; // Repoint
const char *fragmentCode[Programs] = {
    0, // Diplane
    "\
    in vec3 normal;\n\
    out vec4 result;\n\
    void main()\n\
    {\n\
    result = vec4(normal, 1.0f);\n\
    }\n", // Dipoint
    0, // Coplane
    0, // Copoint
    0, // Adplane
    0, // Adpoint
    0, // Perplane
    0, // Perpoint
    0, // Replane
    0}; // Repoint
GLuint Window::compileShader(GLenum type, const char *source)
{
    GLuint ident = glCreateShader(type); glShaderSource(ident, 1, &source, NULL); glCompileShader(ident);
    GLint status = GL_FALSE; glGetShaderiv(ident, GL_COMPILE_STATUS, &status);
    int length; glGetShaderiv(ident, GL_INFO_LOG_LENGTH, &length); if (length > 0) {
    GLchar info[length+1]; glGetShaderInfoLog(ident, length, NULL, info); std::cerr << info << std::endl;}
    return ident;
}
GLuint Window::compileProgram(const char *vertex, const char *geometry, const char *fragment)
{
    GLuint ident = glCreateProgram();
    GLuint vertexIdent = compileShader(GL_VERTEX_SHADER, vertex); glAttachShader(ident, vertexIdent);
    GLuint geometryIdent = compileShader(GL_GEOMETRY_SHADER, geometry); glAttachShader(ident, geometryIdent);
    GLuint fragmentIdent = compileShader(GL_FRAGMENT_SHADER, fragment); glAttachShader(ident, fragmentIdent);
    glLinkProgram(ident);
    GLint status = GL_FALSE; glGetShaderiv(ident, GL_LINK_STATUS, &status);
    int length; glGetShaderiv(ident, GL_INFO_LOG_LENGTH, &length); if (length > 0) {
    GLchar info[length+1]; glGetProgramInfoLog(ident, length, NULL, info); std::cerr << info << std::endl;}
    glDetachShader(ident, vertexIdent); glDeleteShader(vertexIdent);
    glDeleteShader(geometryIdent); glDetachShader(ident, geometryIdent);
    glDetachShader(ident, fragmentIdent); glDeleteShader(fragmentIdent);
    return ident;
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
    if (gl3wInit()) {std::cerr << "Failed to initialize OpenGL" << std::endl; glfwTerminate(); glfwTerminate(); exit(-1);}
    if (!gl3wIsSupported(3, 3)) {std::cerr << "OpenGL 3.3 not supported\n" << std::endl; glfwTerminate(); exit(-1);}
    std::cout << "OpenGL " << glGetString(GL_VERSION) << " GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window);
    while (testGoon) glfwWaitEvents();
    glfwTerminate();
}

void Window::wake()
{
	glfwPostEmptyEvent();
}

