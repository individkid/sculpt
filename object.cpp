/*
*    object.cpp initialize per file information
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

#include "object.hpp"
#include "message.hpp"
#include "write.hpp"
#include "polytope.hpp"
#include "read.hpp"

void Object::initFile(int first)
{
	for (Buffer b = (Buffer)0; b < Buffers; b = (Buffer)((int)b+1))
	initHandle(b,(first),handle[b]);
	for (Program p = (Program)0; p < Programs; p = (Program)((int)p+1))
	glGenVertexArrays(1, &vao[p]);
	for (Buffer b = (Buffer)0; b < Buffers; b = (Buffer)((int)b+1))
	for (Program p = (Program)0; p < Programs; p = (Program)((int)p+1))
	initVao(b,p,vao[p],handle[b].handle);
}

void Object::initHandle(enum Buffer buffer, int first, Handle &handle)
{
	switch (buffer) {
	case (Texture0): handle.unit = GL_TEXTURE0; break;
    case (Texture1): handle.unit = GL_TEXTURE1; break;
    case (Query): glGenQueries(1,&handle.handle); break;
    case (Uniform): if (!first) {handle.handle = 0; break;}
	default: glGenBuffers(1,&handle.handle); break;}
    switch (buffer) {
    case (Facet): case (Element): handle.target = GL_ELEMENT_ARRAY_BUFFER; break;
    case (Vector): handle.target = GL_TRANSFORM_FEEDBACK_BUFFER; break;
    case (Uniform): handle.target = GL_UNIFORM_BUFFER; break;
    case (Texture0): case (Texture1): handle.target = GL_TEXTURE_2D; break;
    case (Query): handle.target = GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN; break;
    default: handle.target = GL_ARRAY_BUFFER; break;}
    switch (buffer) {
    case (Vector): handle.usage = GL_STATIC_READ; break;
    default: handle.usage = GL_STATIC_DRAW; break;}
    switch (buffer) {
    default: handle.index = 0;}
}

void Object::initVao(enum Buffer buffer, enum Program program, MYuint vao, MYuint handle)
{
	glBindVertexArray(vao);
	switch (program) {
	case (Draw): switch (buffer) {
	case (Facet): glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle); break;
	case (Point): initVao3f(0,handle); break;
	case (Normal): for (int i = 1; i < 4; i++) initVao3f(i,handle); break;
	case (Coordinate): for (int i = 4; i < 7; i++) initVao2f(i,handle); break;
	case (Weight): for (int i = 7; i < 10; i++) initVao4u(i,handle); break;
	case (Color): for (int i = 10; i < 13; i++) initVao3f(i,handle); break;
	case (Tag): for (int i = 13; i < 16; i++) initVao2u(i,handle); break;
	default: break;}
    case (Intersect): break;
    case (Regard): break;
    case (Pierce): break;
	default: error("invalid program",program,__FILE__,__LINE__);}
	glBindVertexArray(0);
}

void Object::initVao3f(MYuint index, MYuint handle)
{
	glBindBuffer(GL_ARRAY_BUFFER,handle);
	glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(index);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void Object::initVao2f(MYuint index, MYuint handle)
{
	glBindBuffer(GL_ARRAY_BUFFER,handle);
	glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(index);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void Object::initVao4u(MYuint index, MYuint handle)
{
	glBindBuffer(GL_ARRAY_BUFFER,handle);
	glVertexAttribIPointer(index, 4, GL_UNSIGNED_INT, 4 * sizeof(unsigned), (void*)0);
	glEnableVertexAttribArray(index);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void Object::initVao2u(MYuint index, MYuint handle)
{
	glBindBuffer(GL_ARRAY_BUFFER,handle);
	glVertexAttribIPointer(index, 2, GL_UNSIGNED_INT, 2 * sizeof(unsigned), (void*)0);
	glEnableVertexAttribArray(index);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
