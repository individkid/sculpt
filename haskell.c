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
#include <sys/errno.h>
#include "haskell.h"

static int debug = 0;

void fatal(const char *str, int wrt, const char *file, int line)
{
    fprintf(stderr,"error: %s; wrt %d; file %s; line %d\n",str,wrt,file,line);
    exit(-1);
}

void error(const char *str, int wrt, const char *file, int line)
{
	if (debug) {if (DEBUG & HASKELL_DEBUG) printf("haskell done\n"); exit(0);}
	fatal(str,wrt,file,line);
}

void setDebug(int val)
{
	debug = val;
}

const char *enamerate(enum Opcode opcode)
{
	switch (opcode) {
	case (ReadOp): return "ReadOp"; 
	case (WriteOp): return "WriteOp"; 
	case (PointerOp): return "PointerOp"; 
	case (FileOp): return "FileOp";
	case (PlaneOp): return "PlaneOp";
	case (ConfOp): return "ConfOp";
	case (SculptOp): return "SculptOp";
	case (ClickOp): return "ClickOp";
	case (MouseOp): return "MouseOp";
	case (RollerOp): return "RollerOp";
	case (TargetOp): return "TargetOp";
	case (TopologyOp): return "TopoologyOp";
	case (FixedOp): return "FixedOp";
	case (BoundariesOp): return "BoundariesOp";
	case (RegionsOp): return "RegionsOp";
	case (PlanesOp): return "PlanesOp";
	case (SidesOp): return "SidesOp";
	case (SideOp): return "SideOp";
	case (InsidesOp): return "InsidesOp";
	case (OutsidesOp): return "OutsidesOp";
	case (InsideOp): return "InsideOp";
	case (OutsideOp): return "OutsideOp";
	case (MatrixOp): return "MatrixOp";
	case (VersorOp): return "VersorOp";
	case (VectorOp): return "VectorOp";
	case (DelayOp): return "DelayOp";
	case (CountOp): return "CountOp";
	case (IdentOp): return "IdentOp";
	case (ValueOp): return "ValueOp";
	case (ScriptOp): return "ScriptOp";
	case (SubconfOp): return "SubconfOp";
	case (SettingOp): return "SettingOp";
	case (TextOp): return "TextOp";
	default: fatal("unknown opcode",opcode,__FILE__,__LINE__);}
	return 0;
}

int enumerate(char *name)
{
	if (strcmp(name,"ReadOp") == 0) return ReadOp; 
	if (strcmp(name,"WriteOp") == 0) return WriteOp; 
	if (strcmp(name,"PointerOp") == 0) return PointerOp; 
	if (strcmp(name,"FileOp") == 0) return FileOp;
	if (strcmp(name,"PlaneOp") == 0) return PlaneOp;
	if (strcmp(name,"ConfOp") == 0) return ConfOp;
	if (strcmp(name,"SculptOp") == 0) return SculptOp;
	if (strcmp(name,"ClickOp") == 0) return ClickOp;
	if (strcmp(name,"MouseOp") == 0) return MouseOp;
	if (strcmp(name,"RollerOp") == 0) return RollerOp;
	if (strcmp(name,"TargetOp") == 0) return TargetOp;
	if (strcmp(name,"TopologyOp") == 0) return TopologyOp;
	if (strcmp(name,"FixedOp") == 0) return FixedOp;
	if (strcmp(name,"BoundariesOp") == 0) return BoundariesOp;
	if (strcmp(name,"RegionsOp") == 0) return RegionsOp;
	if (strcmp(name,"PlanesOp") == 0) return PlanesOp;
	if (strcmp(name,"SidesOp") == 0) return SidesOp;
	if (strcmp(name,"SideOp") == 0) return SideOp;
	if (strcmp(name,"InsidesOp") == 0) return InsidesOp;
	if (strcmp(name,"OutsidesOp") == 0) return OutsidesOp;
	if (strcmp(name,"InsideOp") == 0) return InsideOp;
	if (strcmp(name,"OutsideOp") == 0) return OutsideOp;
	if (strcmp(name,"MatrixOp") == 0) return MatrixOp;
	if (strcmp(name,"VersorOp") == 0) return VersorOp;
	if (strcmp(name,"VectorOp") == 0) return VectorOp;
	if (strcmp(name,"DelayOp") == 0) return DelayOp;
	if (strcmp(name,"CountOp") == 0) return CountOp;
	if (strcmp(name,"IdentOp") == 0) return IdentOp;
	if (strcmp(name,"ValueOp") == 0) return ValueOp;
	if (strcmp(name,"ScriptOp") == 0) return ScriptOp;
	if (strcmp(name,"SubconfOp") == 0) return SubconfOp;
	if (strcmp(name,"SettingOp") == 0) return SettingOp;
	if (strcmp(name,"TextOp") == 0) return TextOp;
	if (strcmp(name,"TestConf") == 0) return TestConf;
	fatal("unknown name",0,__FILE__,__LINE__);
	return -1;
}

void *rdPointer(int fd)
{
	void *ptr;
	if (read(fd,&ptr,sizeof(ptr)) != sizeof(ptr)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdPointer: %d %p\n",getpid(),ptr);
	return ptr;
}

int rdOpcode(int fd)
{
	enum Opcode opcode;
	if (read(fd,&opcode,sizeof(opcode)) != sizeof(opcode)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdOpcode: %d %s\n",getpid(),enamerate(opcode));
	return opcode;
}

int rdConfigure(int fd)
{
	enum Configure conf;
	if (read(fd,&conf,sizeof(conf)) != sizeof(conf)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdConfigure: %d %d\n",getpid(),conf);
	return conf;
}

int rdTopologyMode(int fd)
{
	enum TopologyMode topology;
	if (read(fd,&topology,sizeof(topology)) != sizeof(topology)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdTopologyMode: %d %d\n",getpid(),topology);
	return topology;
}

int rdFixedMode(int fd)
{
	enum FixedMode fixed;
	if (read(fd,&fixed,sizeof(fixed)) != sizeof(fixed)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdFixedMode: %d %d\n",getpid(),fixed);
	return fixed;
}

int rdSubconf(int fd)
{
	enum Subconf subconf;
	if (read(fd,&subconf,sizeof(subconf)) != sizeof(subconf)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdSubconf: %d %d\n",getpid(),subconf);
	return subconf;
}

int rdInt(int fd)
{
	int val;
	if (read(fd,&val,sizeof(val)) != sizeof(val)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdInt: %d %d\n",getpid(),val);
	return val;
}

float rdFloat(int fd)
{
	float val;
	if (read(fd,&val,sizeof(val)) != sizeof(val)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdFloat: %d %f\n",getpid(),val);
	return val;
}

double rdDouble(int fd)
{
	double val;
	if (read(fd,&val,sizeof(val)) != sizeof(val)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdDouble: %d %f\n",getpid(),val);
	return val;
}

void rdChars(int fd, int count, char *chars)
{
	if (read(fd,chars,count*sizeof(*chars)) != count*sizeof(*chars)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdChars %d %d\n",getpid(),count);
}

void rdInts(int fd, int count, int *ints)
{
	if (read(fd,ints,count*sizeof(*ints)) != count*sizeof(*ints)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdInts %d %d\n",getpid(),count);
}

void rdFloats(int fd, int count, float *floats)
{
	if (read(fd,floats,count*sizeof(*floats)) != count*sizeof(*floats)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdFloats %d %d\n",getpid(),count);
}

void rdDoubles(int fd, int count, double *doubles)
{
	if (read(fd,doubles,count*sizeof(*doubles)) != count*sizeof(*doubles)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdDoubles %d %d\n",getpid(),count);
}

void wrPointer(int fd, void *ptr)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrPointer %d %p\n",getpid(),ptr);
	if (write(fd,&ptr,sizeof(ptr)) != sizeof(ptr)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrOpcode(int fd, int val)
{
	enum Opcode opcode = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrOpcode: %d %s\n",getpid(),enamerate(opcode));
	if (write(fd,&opcode,sizeof(opcode)) != sizeof(opcode)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrConfigure(int fd, int val)
{
	enum Configure conf = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrConfigure %d %d\n",getpid(),val);
	if (write(fd,&conf,sizeof(conf)) != sizeof(conf)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrTopologyMode(int fd, int val)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrTopologyMode %d %d\n",getpid(),val);
	enum TopologyMode topology = val;
	if (write(fd,&topology,sizeof(topology)) != sizeof(topology)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrFixedMode(int fd, int val)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrFixedMode %d %d\n",getpid(),val);
	enum FixedMode fixed = val;
	if (write(fd,&fixed,sizeof(fixed)) != sizeof(fixed)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrSubconf(int fd, int val)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrSubconf %d %d\n",getpid(),val);
	enum Subconf subconf = val;
	if (write(fd,&subconf,sizeof(subconf)) != sizeof(subconf)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrInt(int fd, int val)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrInt %d %d\n",getpid(),val);
	if (write(fd,&val,sizeof(val)) != sizeof(val)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrFloat(int fd, float val)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrFloat %d %f\n",getpid(),val);
	if (write(fd,&val,sizeof(val)) != sizeof(val)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrDouble(int fd, double val)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrDouble %d %f\n",getpid(),val);
	if (write(fd,&val,sizeof(val)) != sizeof(val)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrChars(int fd, int count, char *chars)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrChars %d %p\n",getpid(),chars);
	if (write(fd,chars,count*sizeof(*chars)) != count*sizeof(*chars)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrInts(int fd, int count, int *ints)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrInts %d %p\n",getpid(),ints);
	if (write(fd,ints,count*sizeof(*ints)) != count*sizeof(*ints)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrFloats(int fd, int count, float *floats)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrFloats %d %p\n",getpid(),floats);
	if (write(fd,floats,count*sizeof(*floats)) != count*sizeof(*floats)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrDoubles(int fd, int count, double *doubles)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrDoubles %d %p\n",getpid(),doubles);
	if (write(fd,doubles,count*sizeof(*doubles)) != count*sizeof(*doubles)) fatal("write failed",errno,__FILE__,__LINE__);
}

void exOpcode(int fd, int opcode)
{
	if (DEBUG & HASKELL_DEBUG) printf("exOpcode %d %s\n",getpid(),enamerate(opcode));
	if (rdOpcode(fd) != opcode) fatal("unexpected opcode",errno,__FILE__,__LINE__);
}

char *hello(char *hello)
{
   static char buffer[100];
   return strcat(strcpy(buffer,hello)," ok again");
}

