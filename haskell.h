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

void setDebug(int val);
int enumerate(char *name);

void *rdPointer(int fd);
int rdOpcode(int fd);
int rdSource(int fd);
int rdConfigure(int fd);
int rdEvent(int fd);
int rdChange(int fd);
int rdGiven(int fd);
int rdSubconf(int fd);
int rdSculpt(int fd);
int rdClickMode(int fd);
int rdMouseMode(int fd);
int rdRollerMode(int fd);
int rdTargetMode(int fd);
int rdTopologyMode(int fd);
int rdFixedMode(int fd);
int rdField(int fd);
int rdBuffer(int fd);
int rdProgram(int fd);
int rdFunction(int fd);
int rdEquate(int fd);
int rdFactor(int fd);
char rdChar(int fd);
int rdInt(int fd);
float rdFloat(int fd);
double rdDouble(int fd);

void wrPointer(int fd, void *val);
void wrOpcode(int fd, int val);
void wrSource(int fd, int val);
void wrConfigure(int fd, int val);
void wrEvent(int fd, int val);
void wrChange(int fd, int val);
void wrGiven(int fd, int val);
void wrSubconf(int fd, int val);
void wrSculpt(int fd, int val);
void wrClickMode(int fd, int val);
void wrMouseMode(int fd, int val);
void wrRollerMode(int fd, int val);
void wrTargetMode(int fd, int val);
void wrTopologyMode(int fd, int val);
void wrFixedMode(int fd, int val);
void wrField(int fd, int val);
void wrBuffer(int fd, int val);
void wrProgram(int fd, int val);
void wrFunction(int fd, int val);
void wrEquate(int fd, int val);
void wrFactor(int fd, int val);
void wrChar(int fd, char val);
void wrInt(int fd, int val);
void wrFloat(int fd, float val);
void wrDouble(int fd, double val);

void exOpcode(int fd, int op);

