/*
*    haskell.h ffi for transfering c structs between pipes and haskell
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

#include "types.h"

void *rdPointer(int fd);
int rdOpcode(int fd);
int rdConfigure(int fd);
int rdTopologyMode(int fd);
int rdFixedMode(int fd);
int rdSubconf(int fd);
int rdInt(int fd);
float rdFloat(int fd);
double rdDouble(int fd);
void rdChars(int fd, int count, char *chars);
void rdInts(int fd, int count, int *ints);
void rdFloats(int fd, int count, float *floats);
void rdDoubles(int fd, int count, double *doubles);
void wrPointer(int fd, void *val);
void wrOpcode(int fd, int val);
void wrConfigure(int fd, int val);
void wrTopologyMode(int fd, int val);
void wrFixedMode(int fd, int val);
void wrSubconf(int fd, int val);
void wrInt(int fd, int val);
void wrFloat(int fd, float val);
void wrDouble(int fd, double val);
void wrChars(int fd, int count, char *chars);
void wrInts(int fd, int count, int *ints);
void wrFloats(int fd, int count, float *floats);
void wrDoubles(int fd, int count, double *doubles);
void exOpcode(int fd, int op);
