/*
*    microcode.cpp initialize microcode
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

#include "microcode.hpp"
#include "message.hpp"

void Microcode::initProgram(Program program)
{
    switch (program) {
    case (Draw): initDraw(); break;
    case (Pierce): initPierce(); break;
    case (Sect0): initSect0(); break;
    case (Sect1): initSect0(); break;
    case (Side1): case (Side2): initSide(); break;
    default: error("invalid program",program,__FILE__,__LINE__);}
}

void Microcode::initDraw()
{
    const char *vertex = "\
    layout (location = 0) in vec3 point;\n\
    layout (location = 1) in vec3 normal[3];\n\
    layout (location = 4) in vec2 coordinate[3];\n\
    layout (location = 7) in vec3 color[3];\n\
    layout (location = 10) in vec3 weight[3];\n\
    layout (location = 13) in uvec2 tag[3];\n\
    out data {\n\
        vec3 normal;\n\
        vec2 coordinate;\n\
        vec3 color;\n\
        vec3 weight;\n\
    } od;\n\
    void main()\n\
    {\n\
        vec3 vector = (affine*vec4(point,1.0)).xyz;\n\
        vector.x = vector.x/(vector.z*slope+1.0);\n\
        vector.y = vector.y/(vector.z*slope*aspect+aspect);\n\
        vector.z = vector.z/cutoff;\n\
        gl_Position = vec4(vector,1.0);\n\
        uint index;\n\
        if (taggraph == tag[0].x) index = 0u;\n\
        else if (taggraph == tag[1].x) index = 1u;\n\
        else index = 2u;\n\
        od.normal = normal[index];\n\
        od.coordinate = coordinate[index];\n\
        od.color = color[index];\n\
        od.weight = weight[index];\n\
    }\n";
    const char *fragment = "\
    in data {\n\
        vec3 normal;\n\
        vec2 coordinate;\n\
        vec3 color;\n\
        vec3 weight;\n\
    } id;\n\
    out vec4 color;\n\
    void main()\n\
    {\n\
        color.xyz = id.color;\n\
        color.w = 1.0;\n\
    }\n";
    initConfigure(vertex,0,fragment,0,0);
}

void Microcode::initPierce()
{
    const char *vertex = "\
    layout (location = 0) in vec3 point;\n\
    layout (location = 1) in vec3 normal[3];\n\
    layout (location = 4) in vec2 coordinate[3];\n\
    layout (location = 7) in uvec2 tag[3];\n\
    out data {\n\
        vec3 point;\n\
        vec3 normal;\n\
        uvec2 tag;\n\
    } od;\n\
    void main()\n\
    {\n\
        vec3 vector = (affine*vec4(point,1.0)).xyz;\n\
        vector.x = vector.x/(vector.z*slope+1.0);\n\
        vector.y = vector.y/(vector.z*slope*aspect+aspect);\n\
        vector.z = vector.z/cutoff;\n\
        od.point = vector;\n\
        uint index;\n\
        if (taggraph == tag[0].x) index = 0u;\n\
        else if (taggraph == tag[1].x) index = 1u;\n\
        else index = 2u;\n\
        od.normal = normal[index];\n\
        od.tag = tag[index];\n\
    }\n";
    const char *geometry = "\
    layout (triangles) in;\n\
    layout (points, max_vertices = 1) out;\n\
    in data {\n\
        vec3 point;\n\
        vec3 normal;\n\
        uvec2 tag;\n\
    } id[3];\n\
    out vec3 pierce;\n\
    out vec3 normal;\n\
    out uint tagbits;\n\
    out uint plane;\n\
    void main()\n\
    {\n\
        // pierce points by feather arrow\n\
        pierce = id[0].point;\n\
        // interpolate normal, etc, by pierce in points\n\
        normal = id[0].normal;\n\
        tagbits = id[0].tag.x;\n\
        plane = id[0].tag.y;\n\
        EmitVertex();\n\
        EndPrimitive();\n\
    }\n";
    const char *feedback[4] = {"pierce","normal","tagbits","plane"};
    initConfigure(vertex,geometry,0,4,feedback);
}

void Microcode::initSect0()
{
    const char *vertex = "\
    layout (location = 0) in vec3 plane;\n\
    layout (location = 1) in uint versor;\n\
    out data {\n\
        vec3 plane;\n\
        uint versor;\n\
    } od;\n\
    void main()\n\
    {\n\
        od.plane = plane;\n\
        od.versor = versor;\n\
    }\n";
    const char *geometry = "\
    layout (triangles) in;\n\
    layout (points, max_vertices = 1) out;\n\
    in data {\n\
        vec3 plane;\n\
        uint versor;\n\
    } id[3];\n\
    out vec3 vertex;\n\
    void main()\n\
    {\n\
        // intersect id into vertex\n\
        if (id[0].versor == 0u) vertex = id[0].plane;\n\
        else vertex = id[1].plane;\n\
        EmitVertex();\n\
        EndPrimitive();\n\
    }\n";
    const char *feedback[1] = {"vertex"};
    initConfigure(vertex,geometry,0,1,feedback);
}

void Microcode::initSect1()
{
    const char *vertex = "\
    layout (location = 0) in vec3 plane;\n\
    out data {\n\
        vec3 plane;\n\
    } od;\n\
    void main()\n\
    {\n\
        od.plane = plane;\n\
    }\n";
    const char *geometry = "\
    layout (triangles) in;\n\
    layout (points, max_vertices = 1) out;\n\
    in data {\n\
        vec3 plane;\n\
    } id[3];\n\
    out vec3 vertex;\n\
    void main()\n\
    {\n\
        // intersect id into vertex\n\
        vertex = id[0].plane;\n\
        EmitVertex();\n\
        EndPrimitive();\n\
    }\n";
    const char *feedback[1] = {"vertex"};
    initConfigure(vertex,geometry,0,1,feedback);
}

void Microcode::initSide()
{
    const char *vertex = "\
    layout (location = 0) in vec3 point;\n\
    out data {\n\
        vec3 point;\n\
    } od;\n\
    void main()\n\
    {\n\
        od.point = point;\n\
    }\n";
    const char *geometry = "\
    layout (triangles) in;\n\
    layout (points, max_vertices = 1) out;\n\
    in data {\n\
        vec3 point;\n\
    } id[3];\n\
    out float scalar;\n\
    void main()\n\
    {\n\
        // subtract feather from arrow pierce\n\
        scalar = id[0].point.x;\n\
        EmitVertex();\n\
        EndPrimitive();\n\
    }\n";
    const char *feedback[1] = {"scalar"};
    initConfigure(vertex,geometry,0,1,feedback);
}

void Microcode::initConfigure(const char *vertex, const char *geometry, const char *fragment,
    int count, const char **feedback)
{
    handle = glCreateProgram();
    MYuint vertexIdent; initShader(GL_VERTEX_SHADER, vertex, vertexIdent); glAttachShader(handle, vertexIdent);
    MYuint geometryIdent; if (geometry) {initShader(GL_GEOMETRY_SHADER, geometry, geometryIdent); glAttachShader(handle, geometryIdent);}
    MYuint fragmentIdent; if (fragment) {initShader(GL_FRAGMENT_SHADER, fragment, fragmentIdent); glAttachShader(handle, fragmentIdent);}
    if (count) glTransformFeedbackVaryings(handle,count,feedback,GL_INTERLEAVED_ATTRIBS);
    glLinkProgram(handle);
    GLint status = GL_FALSE; glGetProgramiv(handle, GL_LINK_STATUS, &status);
    int length; glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length); if (length > 0) {
    GLchar info[length+1]; glGetProgramInfoLog(handle, length, NULL, info); std::cerr << info << std::endl; exit(-1);}
    glDetachShader(handle, vertexIdent); glDeleteShader(vertexIdent);
    if (geometry) {glDetachShader(handle, geometryIdent); glDeleteShader(geometryIdent);}
    glDetachShader(handle, fragmentIdent); glDeleteShader(fragmentIdent);
    glUniformBlockBinding(handle,glGetUniformBlockIndex(handle,"Uniform"),0);
    glUniformBlockBinding(handle,glGetUniformBlockIndex(handle,"Global"),1);
	mode = GL_TRIANGLES;
	primitive = GL_POINTS;
}

void Microcode::initShader(MYenum type, const char *source, MYuint &ident)
{
    const char *code[2] = {
    "#version 330 core\n\
    layout(std140) uniform Uniform {\n\
    mat4 perfile;\n\
    };\n\
    layout(std140) uniform Global {\n\
    vec2 cursor;\n\
    mat4 affine;\n\
    mat4 perplane;\n\
    mat3 basis[3];\n\
    float cutoff;\n\
    float slope;\n\
    float aspect;\n\
    float feather;\n\
    float arrow;\n\
    uint tagplane;\n\
    uint taggraph;\n\
    };",
    source};
    ident = glCreateShader(type); glShaderSource(ident, 2, code, NULL); glCompileShader(ident);
    GLint status = GL_FALSE; glGetShaderiv(ident, GL_COMPILE_STATUS, &status);
    int length; glGetShaderiv(ident, GL_INFO_LOG_LENGTH, &length); if (length > 0) {
    GLchar info[length+1]; glGetShaderInfoLog(ident, length, NULL, info); std::cerr << info << std::endl; exit(-1);}
}
