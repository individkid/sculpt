/*
*    haskell.c ffi for transfering c structs between pipes and haskell
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

#include <string.h>
#include <unistd.h>
#include "types.h"

int enumerate(char *name)
{
	if (strcmp(name,"ReadOp") == 0) return ReadOp;
	if (strcmp(name,"WriteOp") == 0) return WriteOp;
	if (strcmp(name,"ScriptOp") == 0) return ScriptOp;
	if (strcmp(name,"WindowOp") == 0) return WindowOp;
	if (strcmp(name,"CommandOp") == 0) return CommandOp;
	if (strcmp(name,"Opcodes") == 0) return Opcodes;
	return -1;
}

int locate(char *name, char *field)
{
	if (strcmp(name,"Data") == 0 && strcmp(field,"file") == 0) return (int)&(((struct Data*)0)->file);
	return -1;
}

int opcode(int fd)
{
	enum Opcode op;
	if (read(fd,&op,sizeof(op)) != sizeof(op)) return Opcodes;
	return op;
}

char buffer[100];
char *hello(char *hello)
{
   return strcat(strcpy(buffer,hello)," ok again");
}

