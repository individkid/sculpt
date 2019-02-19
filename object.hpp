/*
*    object.hpp initialize per file information
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

#ifndef OBJECT_HPP
#define OBJECT_HPP

extern "C" {
#include "types.h"
}

struct Handle
{
	MYenum target;
	MYenum unit;
	MYuint handle;
	MYenum usage;
	MYuint index;
};
struct Object
{
	Handle handle[Buffers];
	MYuint vao[Programs][Spaces];
	void initFile(int first);
	void initHandle(enum Buffer buffer, int first, Handle &handle);
	void initVao(enum Buffer buffer, enum Program program, enum Space space, MYuint vao, MYuint handle);
	void initVao3f(MYuint index, MYuint handle);
	void initVao2f(MYuint index, MYuint handle);
	void initVao4u(MYuint index, MYuint handle);
	void initVao2u(MYuint index, MYuint handle);
};

#endif
