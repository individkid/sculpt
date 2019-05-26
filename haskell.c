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
#include <stdlib.h>
#include <stdio.h>
#include "haskell.h"

void error(const char *str)
{
    fprintf(stderr,"Haskell error: %s\n",str);
    exit(-1);
}

int enumerate(char *name)
{
	if (strcmp(name,"ReadOp") == 0) return ReadOp; 
	if (strcmp(name,"WriteOp") == 0) return WriteOp; 
	if (strcmp(name,"ScriptOp") == 0) return ScriptOp; 
	if (strcmp(name,"WindowOp") == 0) return WindowOp;
	if (strcmp(name,"FileOp") == 0) return FileOp;
	if (strcmp(name,"PlaneOp") == 0) return PlaneOp;
	if (strcmp(name,"ConfOp") == 0) return ConfOp;
	if (strcmp(name,"Opcodes") == 0) return Opcodes;
	if (strcmp(name,"BoundariesOp") == 0) return BoundariesOp;
	if (strcmp(name,"RegionsOp") == 0) return RegionsOp;
	if (strcmp(name,"PlanesOp") == 0) return PlanesOp;
	if (strcmp(name,"SidesOp") == 0) return SidesOp;
	if (strcmp(name,"SideOp") == 0) return SideOp;
	if (strcmp(name,"InsidesOp") == 0) return InsidesOp;
	if (strcmp(name,"OutsidesOp") == 0) return OutsidesOp;
	if (strcmp(name,"InsideOp") == 0) return InsideOp;
	if (strcmp(name,"OutsideOp") == 0) return OutsideOp;
	if (strcmp(name,"VersorOp") == 0) return VersorOp;
	if (strcmp(name,"VectorOp") == 0) return VectorOp;
	if (strcmp(name,"DelayOp") == 0) return DelayOp;
	if (strcmp(name,"CountOp") == 0) return CountOp;
	if (strcmp(name,"IdentOp") == 0) return IdentOp;
	if (strcmp(name,"ValueOp") == 0) return ValueOp;
	if (strcmp(name,"ScriptOp") == 0) return ScriptOp;
	if (strcmp(name,"TopologyOp") == 0) return TopologyOp;
	if (strcmp(name,"FixedOp") == 0) return FixedOp;
	if (strcmp(name,"PierceOp") == 0) return PierceOp;
	if (strcmp(name,"SeqnumOp") == 0) return SeqnumOp;
	if (strcmp(name,"MatrixOp") == 0) return MatrixOp;
	if (strcmp(name,"SubconfOp") == 0) return SubconfOp;
	if (strcmp(name,"SettingOp") == 0) return SettingOp;
	if (strcmp(name,"TextOp") == 0) return TextOp;
	error("unknown opcode");
	return -1;
}

int rdOpcode(int fd)
{
	enum Opcode opcode;
	if (read(fd,&opcode,sizeof(opcode)) != sizeof(opcode)) error("read failed");
	return opcode;
}

int rdConfigure(int fd)
{
	enum Configure conf;
	if (read(fd,&conf,sizeof(conf)) != sizeof(conf)) error("read failed");
	return conf;
}

int rdTopologyMode(int fd)
{
	enum TopologyMode topology;
	if (read(fd,&topology,sizeof(topology)) != sizeof(topology)) error("read failed");
	return topology;
}

int rdFixedMode(int fd)
{
	enum FixedMode fixed;
	if (read(fd,&fixed,sizeof(fixed)) != sizeof(fixed)) error("read failed");
	return fixed;
}

int rdSubconf(int fd)
{
	enum Subconf subconf;
	if (read(fd,&subconf,sizeof(subconf)) != sizeof(subconf)) error("read failed");
	return subconf;
}

int rdInt(int fd)
{
	int val;
	if (read(fd,&val,sizeof(val)) != sizeof(val)) error("read failed");
	return val;
}

float rdFloat(int fd)
{
	float val;
	if (read(fd,&val,sizeof(val)) != sizeof(val)) error("read failed");
	return val;
}

double rdDouble(int fd)
{
	double val;
	if (read(fd,&val,sizeof(val)) != sizeof(val)) error("read failed");
	return val;
}

void rdChars(int fd, int count, char *chars)
{
	if (read(fd,chars,count*sizeof(*chars)) != count*sizeof(*chars)) error("read failed");
}

void rdInts(int fd, int count, int *ints)
{
	if (read(fd,ints,count*sizeof(*ints)) != count*sizeof(*ints)) error("read failed");
}

void rdFloats(int fd, int count, float *floats)
{
	if (read(fd,floats,count*sizeof(*floats)) != count*sizeof(*floats)) error("read failed");
}

void rdDoubles(int fd, int count, double *doubles)
{
	if (read(fd,doubles,count*sizeof(*doubles)) != count*sizeof(*doubles)) error("read failed");
}

void wrOpcode(int fd, int val)
{
	enum Opcode op = val;
	if (write(fd,&op,sizeof(op)) != sizeof(op)) error("write failed");
}

void wrConfigure(int fd, int val)
{
	enum Configure conf = val;
	if (write(fd,&conf,sizeof(conf)) != sizeof(conf)) error("write failed");
}

void wrTopologyMode(int fd, int val)
{
	enum TopologyMode topology = val;
	if (write(fd,&topology,sizeof(topology)) != sizeof(topology)) error("write failed");
}

void wrFixedMode(int fd, int val)
{
	enum FixedMode fixed = val;
	if (write(fd,&fixed,sizeof(fixed)) != sizeof(fixed)) error("write failed");
}

void wrSubconf(int fd, int val)
{
	enum Subconf subconf = val;
	if (write(fd,&subconf,sizeof(subconf)) != sizeof(subconf)) error("write failed");
}

void wrInt(int fd, int val)
{
	if (write(fd,&val,sizeof(val)) != sizeof(val)) error("write failed");
}

void wrFloat(int fd, float val)
{
	if (write(fd,&val,sizeof(val)) != sizeof(val)) error("write failed");
}

void wrDouble(int fd, double val)
{
	if (write(fd,&val,sizeof(val)) != sizeof(val)) error("write failed");
}

void wrChars(int fd, int count, char *chars)
{
	if (write(fd,chars,count*sizeof(*chars)) != count*sizeof(*chars)) error("write failed");
}

void wrInts(int fd, int count, int *ints)
{
	if (write(fd,ints,count*sizeof(*ints)) != count*sizeof(*ints)) error("write failed");
}

void wrFloats(int fd, int count, float *floats)
{
	if (write(fd,floats,count*sizeof(*floats)) != count*sizeof(*floats)) error("write failed");
}

void wrDoubles(int fd, int count, double *doubles)
{
	if (write(fd,doubles,count*sizeof(*doubles)) != count*sizeof(*doubles)) error("write failed");
}

void exOpcode(int fd, int opcode)
{
	if (rdOpcode(fd) != opcode) error("unexpected opcode");
}

char *hello(char *hello)
{
   static char buffer[100];
   return strcat(strcpy(buffer,hello)," ok again");
}

