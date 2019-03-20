/*
*    microcode.hpp initialize microcode
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

#ifndef MICROCODE_HPP
#define MICROCODE_HPP

extern "C" {
#include "types.h"
}

struct Microcode
{
	MYuint handle;
	MYenum mode;
	MYenum primitive;
	void initProgram(Program program);
	void initDraw();
	void initConfigure(const char *vertex, const char *geometry, const char *fragment, int count, const char **feedback);
	void initShader(MYenum type, const char *source, MYuint &handle);
};

#endif
