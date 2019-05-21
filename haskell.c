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
#include "types.h"

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
	if (strcmp(name,"CommandOp") == 0) return CommandOp;
	if (strcmp(name,"Opcodes") == 0) return Opcodes;
	if (strcmp(name,"RefineConf") == 0) return RefineConf;
	if (strcmp(name,"TweakConf") == 0) return TweakConf;
	if (strcmp(name,"TransformConf") == 0) return TransformConf;
	if (strcmp(name,"PlaneConf") == 0) return PlaneConf;
	if (strcmp(name,"SpaceConf") == 0) return SpaceConf;
	if (strcmp(name,"RegionConf") == 0) return RegionConf;
	if (strcmp(name,"InflateConf") == 0) return InflateConf;
	if (strcmp(name,"PictureConf") == 0) return PictureConf;
	if (strcmp(name,"TestConf") == 0) return TestConf;
	if (strcmp(name,"Configures") == 0) return Configures;
	if (strcmp(name,"NumericMode") == 0) return NumericMode;
	if (strcmp(name,"InvariantMode") == 0) return InvariantMode;
	if (strcmp(name,"SymbolicMode") == 0) return SymbolicMode;
	if (strcmp(name,"TopologyModes") == 0) return TopologyModes;
	if (strcmp(name,"RelativeMode") == 0) return RelativeMode;
	if (strcmp(name,"AbsoluteMode") == 0) return AbsoluteMode;
	if (strcmp(name,"FixedModes") == 0) return FixedModes;
	return -1;
}

int identify(char *name)
{
	if (strcmp(name,"Opcode") == 0) return OpcodeIdent;
	if (strcmp(name,"Int") == 0) return IntIdent;
	if (strcmp(name,"Float") == 0) return FloatIdent;
	if (strcmp(name,"Data") == 0) return DataIdent;
	if (strcmp(name,"Command") == 0) return CommandIdent;
	return -1;
}

int location(int ident, char *field)
{
	switch (ident) {
	case (DataIdent):
	if (strcmp(field,"file") == 0) return FileLoc;
	if (strcmp(field,"plane") == 0) return PlaneLoc;
	if (strcmp(field,"conf") == 0) return ConfLoc;
	if (strcmp(field,"boundaries") == 0) return BoundariesLoc;
	if (strcmp(field,"regions") == 0) return RegionsLoc;
	if (strcmp(field,"planes") == 0) return PlanesLoc;
	if (strcmp(field,"sides") == 0) return SidesLoc;
	if (strcmp(field,"side") == 0) return SideLoc;
	if (strcmp(field,"insides") == 0) return InsidesLoc;
	if (strcmp(field,"outsides") == 0) return OutsidesLoc;
	if (strcmp(field,"inside") == 0) return InsideLoc;
	if (strcmp(field,"outside") == 0) return OutsideLoc;
	if (strcmp(field,"versor") == 0) return VersorLoc;
	if (strcmp(field,"vector") == 0) return VectorLoc;
	if (strcmp(field,"topology") == 0) return TopologyLoc;
	if (strcmp(field,"fixed") == 0) return FixedLoc;
	if (strcmp(field,"pierce") == 0) return PierceLoc;
	if (strcmp(field,"seqnum") == 0) return SeqnumLoc;
	if (strcmp(field,"matrix") == 0) return MatrixLoc;
	if (strcmp(field,"text") == 0) return TextLoc;
	default: error("invalid identity");}
	return -1;
}

int structure(int ident)
{
	switch (ident) {
	case (OpcodeIdent): return sizeof(enum Opcode);
	case (IntIdent): return sizeof(int);
	case (FloatIdent): return sizeof(float);
	case (DataIdent): return sizeof(struct Data);
	case (CommandIdent): return sizeof(struct Command);
	default: error("invalid identity");}
	return 0;
}

#define DATA_FIELD(FIELD) (((struct Data *)str)->FIELD)
#define COMMAND_FIELD(FIELD) (((struct Command *)str)->FIELD)
int *integral(char *str, int ident, int loc)
{
	switch (ident) {
	case (IntIdent): return ((int *)str) + loc;
	case (DataIdent): switch (loc) {
	case (FileLoc): return &DATA_FIELD(file);
	case (PlaneLoc): return &DATA_FIELD(plane);
	case (BoundariesLoc): return &DATA_FIELD(boundaries);
	case (RegionsLoc): return &DATA_FIELD(regions);
	case (SideLoc): return &DATA_FIELD(side);
	case (InsidesLoc): return &DATA_FIELD(insides);
	case (OutsidesLoc): return &DATA_FIELD(outsides);
	case (VersorLoc): return &DATA_FIELD(versor);
	case (SeqnumLoc): return &DATA_FIELD(seqnum);
	default: error("invalid location");} break;
 	case (CommandIdent): // TODO
	default: error("invalid identity");}
	return 0;
}

float *scalar(char *str, int ident, int loc)
{
	switch (ident) {
	case (FloatIdent): return ((float *)str) + loc;
	default: error("invalid identity");}
	return 0;
}

char *character(char *str, int ident, int loc)
{
	switch (ident) {
	case (OpcodeIdent): return str+loc;
	case (DataIdent): switch (loc) {
	case (ConfLoc): return str+loc;
	case (PlanesLoc): return str+loc;
	case (SidesLoc): return str+loc;
	case (InsideLoc): return str+loc;
	case (OutsideLoc): return str+loc;
	case (VectorLoc): return str+loc;
	case (TopologyLoc): return str+loc;
	case (FixedLoc): return str+loc;
	case (PierceLoc): return str+loc;
	case (MatrixLoc): return str+loc;
	case (TextLoc): return str+loc;
	default: error("invalid location");} break;
 	case (CommandIdent): // TODO
	default: error("invalid identity");}
	return 0;
}

void fromchar(char *str, int ident, int loc)
{
	union Symbol symbol; symbol.count = -1;
	switch (ident) {
	case (OpcodeIdent): *(enum Opcode *)str = *str; break;
	case (DataIdent): switch (loc) {
	case (ConfLoc): DATA_FIELD(conf) = *(int *)str; break;
	case (PlanesLoc): symbol.count = *(int *)str; DATA_FIELD(planes) = symbol.inum; break;
	case (SidesLoc): symbol.count = *(int *)str; DATA_FIELD(sides) = symbol.inum; break;
	case (InsideLoc): symbol.count = *(int *)str; DATA_FIELD(inside) = symbol.inum; break;
	case (OutsideLoc): symbol.count = *(int *)str; DATA_FIELD(outside) = symbol.inum; break;
	case (VectorLoc): symbol.count = *(int *)str; DATA_FIELD(vector) = symbol.fnum; break;
	case (TopologyLoc): DATA_FIELD(topology) = *(int *)str; break;
	case (FixedLoc): DATA_FIELD(fixed) = *(int *)str; break;
	case (PierceLoc): symbol.count = *(int *)str; DATA_FIELD(pierce) = symbol.fnum; break;
	case (MatrixLoc): symbol.count = *(int *)str; DATA_FIELD(matrix) = symbol.fnum; break;
	case (TextLoc): symbol.count = *(int *)str; DATA_FIELD(text) = symbol.text; break;
	default: error("invalid location");} break;
 	case (CommandIdent): // TODO
	default: error("invalid identity");}
}

void tochar(char *str, int ident, int loc)
{
	union Symbol symbol; symbol.count = -1;
	switch (ident) {
	case (OpcodeIdent): *str = *(enum Opcode *)str; break;
	case (DataIdent): switch (loc) {
	case (ConfLoc): *str = DATA_FIELD(conf); break;
	case (PlanesLoc): symbol.inum = DATA_FIELD(planes); *str = symbol.count; break;
	case (SidesLoc): symbol.inum = DATA_FIELD(sides); *str = symbol.count; break;
	case (InsideLoc): symbol.inum = DATA_FIELD(inside); *str = symbol.count; break;
	case (OutsideLoc): symbol.inum = DATA_FIELD(outside); *str = symbol.count; break;
	case (VectorLoc): symbol.fnum = DATA_FIELD(vector); *str = symbol.count; break;
	case (TopologyLoc): *str = DATA_FIELD(topology); break;
	case (FixedLoc): *str = DATA_FIELD(fixed); break;
	case (PierceLoc): symbol.fnum = DATA_FIELD(pierce); *str = symbol.count; break;
	case (MatrixLoc): symbol.fnum = DATA_FIELD(matrix); *str = symbol.count; break;
	case (TextLoc): symbol.text = DATA_FIELD(text); *str = symbol.count; break;
	default: error("invalid location");} break;
 	case (CommandIdent): // TODO
	default: error("invalid identity");}
}

int replication(char *str, int ident, int loc)
{
	union Symbol symbol; symbol.count = -1;
	switch (ident) {
	case (DataIdent): switch (loc) {
	case (PlanesLoc): symbol.inum = DATA_FIELD(planes); break;
	case (SidesLoc): symbol.inum = DATA_FIELD(sides); break;
	case (InsideLoc): symbol.inum = DATA_FIELD(inside); break;
	case (OutsideLoc): symbol.inum = DATA_FIELD(outside); break;
	case (VectorLoc): symbol.fnum = DATA_FIELD(vector); break;
	case (PierceLoc): symbol.fnum = DATA_FIELD(pierce); break;
	case (MatrixLoc): symbol.fnum = DATA_FIELD(matrix); break;
	case (TextLoc): symbol.text = DATA_FIELD(text); break;
 	default: error("invalid location");} break;
 	case (CommandIdent): // TODO
	default: error("invalid identity");}
	return symbol.count;
}

#define DATA_SIZEOF(FIELD) (sizeof(*DATA_FIELD(FIELD)))
#define DATA_OFFSET0(FIELD0) (\
	sizeof(struct Data)+\
	DATA_SIZEOF(FIELD0)*sub)
#define DATA_OFFSET1(FIELD0,LOC0,FIELD1) (\
	sizeof(struct Data)+\
	DATA_SIZEOF(FIELD0)*replication(str,ident,LOC0)+\
	DATA_SIZEOF(FIELD1)*sub)
#define COMMAND_SIZEOF(FIELD) (sizeof(*COMMAND_FIELD(FIELD)))
int reference(char *str, int ident, int loc, int sub)
{
	switch (ident) {
	case (DataIdent): switch (loc) {
	case (PlanesLoc): return DATA_OFFSET0(planes);
	case (SidesLoc): return DATA_OFFSET1(planes,PlanesLoc,sides);
	case (InsideLoc): return DATA_OFFSET0(inside);
	case (OutsideLoc): return DATA_OFFSET1(inside,InsideLoc,outside);
	case (VectorLoc): return DATA_OFFSET0(vector);
	case (PierceLoc): return DATA_OFFSET0(pierce);
	case (MatrixLoc): return DATA_OFFSET0(matrix);
	case (TextLoc): return DATA_OFFSET0(text);
 	default: error("invalid location");} break;
 	case (CommandIdent): // TODO
	default: error("invalid identity");}
	return 0;
}

#define DATA_EXTENT0(FIELD0,LOC0) (\
	DATA_SIZEOF(FIELD0)*replication(str,ident,LOC0))
#define DATA_EXTENT1(FIELD0,LOC0,FIELD1,LOC1) (\
	DATA_SIZEOF(FIELD0)*replication(str,ident,LOC0)+\
	DATA_SIZEOF(FIELD1)*replication(str,ident,LOC1))
int extent(char *str, int ident, int loc)
{
	switch (ident) {
	case (DataIdent): switch (loc) {
	case (PlanesLoc): return DATA_EXTENT0(planes,PlanesLoc);
	case (SidesLoc): return DATA_EXTENT1(planes,PlanesLoc,sides,SidesLoc);
	case (InsideLoc): return DATA_EXTENT0(inside,InsideLoc);
	case (OutsideLoc): return DATA_EXTENT1(inside,InsideLoc,outside,OutsideLoc);
	case (VectorLoc): return DATA_EXTENT0(vector,VectorLoc);
	case (PierceLoc): return DATA_EXTENT0(pierce,PierceLoc);
	case (MatrixLoc): return DATA_EXTENT0(matrix,MatrixLoc);
	case (TextLoc): return DATA_EXTENT0(text,TextLoc);
 	default: error("invalid location");} break;
 	case (CommandIdent): // TODO
	default: error("invalid identity");}
	return 0;
}

void input(int fd, int size, char *str)
{
	if (read(fd,str,size) != size) error("read failed");
}

void output(int fd, int size, char *str)
{
	if (write(fd,str,size) != size) error("write failed");
}

char *hello(char *hello)
{
   static char buffer[100];
   return strcat(strcpy(buffer,hello)," ok again");
}

