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
    case (Sect1): initSect1(); break;
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
        if (taggraph == (tag[0].x&7u)) index = 0u;\n\
        else if (taggraph == (tag[1].x&7u)) index = 1u;\n\
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
    uniform sampler2D texture0;\n\
    uniform sampler2D texture1;\n\
    void main()\n\
    {\n\
        vec4 color0;\n\
        color0.xyz = id.color;\n\
        color0.w = 1.0;\n\
        vec4 color1 = texture(texture0,id.coordinate);\n\
        vec4 color2 = texture(texture1,id.coordinate);\n\
        color = color0*id.weight.x+color1*id.weight.y+color2*id.weight.z;\n\
    }\n";
    initConfigure(vertex,0,fragment,0,0);
}

static const char *intersect = "\
    // minimum difference between first and other points wrt versor\n\
    void minimum(in vec3 point0, in vec3 point1, in vec3 point2, in uint versor, out float result)\n\
    {\n\
        float diff0, diff1;\n\
        diff0 = point1[versor] - point0[versor];\n\
        diff1 = point2[versor] - point0[versor];\n\
        if (abs(diff0) < abs(diff1)) result = diff0; else result = diff1;\n\
    }\n\
    // minimum distance of index[0] to others wrt versor is maximum\n\
    void maximum(in mat3 point, in uint versor, out uint index[3])\n\
    {\n\
        vec3 diff;\n\
        uint vertex;\n\
        for (uint i = 0u; i < 3u; i++)\n\
            minimum(point[i],point[(i+1u)%3u],point[(i+2u)%3u],versor,diff[i]);\n\
        vertex = 0u;\n\
        for (uint i = 1u; i < 3u; i++)\n\
            if (abs(diff[i]) < abs(diff[vertex])) vertex = i;\n\
        for (uint i = 0u; i < 3u; i++) {\n\
            index[i] = vertex;\n\
            vertex = (vertex + 1u) % 3u;\n\
        }\n\
    }\n\
    // project point onto plane\n\
    void project(in mat3 plane, in uint versor, out vec3 solve)\n\
    {\n\
        // x0*s+y0*t+u=z0\n\
        // x1*s+y1*t+u=z1\n\
        // x2*s+y2*t+u=z2\n\
        // (x1-x0)*s+(y1-y0)*t=z1-z0 d1.xy*(s,t)=d1.z\n\
        // (x2-x0)*s+(y2-y0)*t=z2-z0 d2.xy*(s,t)=d2.z\n\
        // ((x1-x0)/(y1-y0))*s+t=((z1-z0)/(y1-y0)) (d1.x/d1.y)*s+t=d1.z/d1.y e1*s+t=f1\n\
        // ((x2-x0)/(y2-y0))*s+t=((z2-z0)/(y2-y0)) (d2.x/d2.y)*s+t=d2.z/d2.y e2*s+t+f2\n\
        // ((x2-x0)/(y2-y0)-(x1-x0)/(y1-y0))*s=((z2-z0)/(y2-y0)-(z1-z0)/(y1-y0)) (e2-e1)*s=(f2-f1) g1*s=g2\n\
        // s=((z2-z0)/(y2-y0)-(z1-z0)/(y1-y0))/((x2-x0)/(y2-y0)-(x1-x0)/(y1-y0)) s=g2/g1\n\
        // t=((z1-z0)/(y1-y0))-((x2-x0)/(y2-y0))*s t=f1-e2*s\n\
        // u=z0-y0*t-x0*s\n\
        vec3 v0, v1, v2; // (x0,y0,z0), (x1,y1,z1), (x2,y2,z2)\n\
        vec3 d1, d2; // (v1-v0), (v2-v0)\n\
        float e1, e2; // d1[0]/d1[1], d2[0]/d2[1]\n\
        float f1, f2; // d1[2]/d1[1], d2[2]/d2[1]\n\
        float g1, g2; // e2-e1, f2-f1\n\
        float s, t, u; // g2/g1, f1-e2 z0-y0*t-x0*s\n\
        uint index[3];\n\
        for (uint i = 0u; i < 3u; i++) {\n\
            index[i] = versor;\n\
            versor = (versor + 1u) % 3u;\n\
        }\n\
        v0 = plane[index[0]]; v1 = plane[index[1]]; v2 = plane[index[2]];\n\
        d1 = v1-v0; d2 = v2-v0;\n\
        e1 = d1.x/d1.y; e2 = d2.x/d2.y;\n\
        f1 = d1.z/d1.y; f2 = d2.z/d2.y;\n\
        g1 = e2-e1; g2 = f2-f1;\n\
        solve.x = g2/g1; solve.y = f1-e2; solve.z = v0.z-v0.y*t-v0.x*s;\n\
    }\n\
    // find intersecton between line through points and plane\n\
    void intersect(in vec3 solve, in uint versor, in vec3 point0, in vec3 point1, out vec3 point)\n\
    {\n\
        float scalar0, scalar1;\n\
        vec3 diff = point1 - point0;\n\
        scalar0 = point0.x*solve.x+point0.y*solve.y+solve.z - point0[versor];\n\
        scalar1 = point1[versor] - point1.x*solve.x+point1.y*solve.y+solve.z;\n\
        point = point0 + (diff * (scalar0 / (scalar0 + scalar1)));\n\
    }\n";

static const char *dimension = "\
void dimension(in mat3 point, out uint versor)\n\
{\n\
    vec3 dif, max, min;\n\
    versor = 0u;\n\
    for (uint i = 0u; i < 3u; i++)\n\
    for (uint j = 0u; j < 3u; j++)\n\
    if (i == 0u) min[j] = max[j] = point[i][j]; else {\n\
        if (point[i][j] < min[j]) min[j] = point[i][j];\n\
        if (point[i][j] > max[j]) max[j] = point[i][j];\n\
    }\n\
    dif = max-min;\n\
    versor = 0u;\n\
    for (uint i = 0u; i < 3u; i++)\n\
        if (abs(dif[i]) < abs(dif[versor])) versor = i;\n\
}\n";

void Microcode::initPierce()
{
    const char *vertex = "\
    layout (location = 0) in vec3 point;\n\
    layout (location = 1) in vec3 normal[3];\n\
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
        if (taggraph == (tag[0].x&7u)) index = 0u;\n\
        else if (taggraph == (tag[1].x&7u)) index = 1u;\n\
        else index = 2u;\n\
        od.normal = normal[index];\n\
        od.tag.x = tag[index].x>>3;\n\
        od.tag.y = tag[index].y;\n\
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
        mat3 point;\n\
        vec3 solve;\n\
        uint versor;\n\
        for (uint i = 0u; i < 3u; i++)\n\
            point[i] = id[i].point;\n\
        dimension(point,versor);\n\
        project(point,versor,solve);\n\
        intersect(solve,versor,vec3(0.0,0.0,-1.0),vec3(cursor,0.0),pierce);\n\
        normal = vec3(0.0,0.0,0.0);\n\
        for (uint i = 0u; i < 3u; i++)\n\
            normal = normal + id[i].normal;\n\
        normal = normal / 3.0;\n\
        tagbits = id[0].tag.x;\n\
        plane = id[0].tag.y;\n\
        EmitVertex();\n\
        EndPrimitive();\n\
    }\n";
    const char *feedback[4] = {"pierce","normal","tagbits","plane"};
    char str[strlen(dimension)+strlen(intersect)+strlen(geometry)+1];
    *str = 0; strcat(strcat(strcat(str,dimension),intersect),geometry);
    initConfigure(vertex,str,0,4,feedback);
}

static const char *convert = "\
    void convert(in vec3 plane, in uint versor, out mat3 point)\n\
    {\n\
        for (uint i = 0u; i < 3u; i++) {\n\
            point[i] = basis[versor][i];\n\
            point[i][versor] = plane[i];\n\
        }\n\
    }\n";

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
        mat3 point[3];\n\
        uint corner[3];\n\
        vec3 pierce[2];\n\
        vec3 solve;\n\
        for (uint i = 0u; i < 3u; i++)\n\
            convert(id[i].plane,id[i].versor,point[i]);\n\
        maximum(point[0],id[1].versor,corner);\n\
        project(point[1],id[1].versor,solve);\n\
        intersect(solve,id[1].versor,point[0][corner[0]],point[0][corner[1]],pierce[0]);\n\
        intersect(solve,id[1].versor,point[0][corner[0]],point[0][corner[2]],pierce[1]);\n\
        project(point[2],id[2].versor,solve);\n\
        intersect(solve,id[2].versor,pierce[0],pierce[1],vertex);\n\
        EmitVertex();\n\
        EndPrimitive();\n\
    }\n";
    const char *feedback[1] = {"vertex"};
    char str[strlen(convert)+strlen(intersect)+strlen(geometry)+1];
    *str = 0; strcat(strcat(strcat(str,convert),intersect),geometry);
    initConfigure(vertex,str,0,1,feedback);
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
        mat3 point[3];\n\
        uint corner[3];\n\
        vec3 pierce[2];\n\
        vec3 solve;\n\
        for (uint i = 0u; i < 3u; i++)\n\
            convert(id[i].plane,0u,point[i]);\n\
        maximum(point[0],0u,corner);\n\
        project(point[1],0u,solve);\n\
        intersect(solve,0u,point[0][corner[0]],point[0][corner[1]],pierce[0]);\n\
        intersect(solve,0u,point[0][corner[0]],point[0][corner[2]],pierce[1]);\n\
        project(point[2],0u,solve);\n\
        intersect(solve,0u,pierce[0],pierce[1],vertex);\n\
        EmitVertex();\n\
        EndPrimitive();\n\
    }\n";
   const char *feedback[1] = {"vertex"};
    char str[strlen(convert)+strlen(intersect)+strlen(geometry)+1];
    *str = 0; strcat(strcat(strcat(str,convert),intersect),geometry);
    initConfigure(vertex,str,0,1,feedback);
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
    layout (points) in;\n\
    layout (points, max_vertices = 1) out;\n\
    in data {\n\
        vec3 point;\n\
    } id[1];\n\
    out float scalar;\n\
    void main()\n\
    {\n\
        vec3 diff = id[0].point-feather;\n\
        scalar = 0.0;\n\
        for (uint i = 0u; i < 3u; i++)\n\
        scalar = scalar + diff[i]*arrow[i];\n\
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
    vec3 feather;\n\
    vec3 arrow;\n\
    uint enable;\n\
    uint tagplane;\n\
    uint taggraph;\n\
    };",
    source};
    ident = glCreateShader(type); glShaderSource(ident, 2, code, NULL); glCompileShader(ident);
    GLint status = GL_FALSE; glGetShaderiv(ident, GL_COMPILE_STATUS, &status);
    int length; glGetShaderiv(ident, GL_INFO_LOG_LENGTH, &length); if (length > 0) {
    GLchar info[length+1]; glGetShaderInfoLog(ident, length, NULL, info); std::cerr << info << std::endl; exit(-1);}
}
