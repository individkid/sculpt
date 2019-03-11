/*
*    parse.c function for converting string to Command
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

#include <stdio.h>
#include <string>
#include "types.h"
#include "message.hpp"

extern "C" void setData(int file, struct Update *update);
extern "C" void getUniform(int file, struct Update *update);
extern "C" void putUniform(int file, struct Update *update);
extern "C" void checkQuery(int file, struct Update *update);

Pool<Command> commands;
Pool<Update> updates;
Pool<Render> renders;
Pool<char> pools[10]; // TODO way to return array of given type

const char *field[] = {"AllocField","WriteField","BindField","ReadField",0};
const char *buffer[] = {
	"Plane", "Versor", "Point", "Normal", "Coordinate", "Weight", "Color", "Tag",
	"Face", "Frame", "Coface", "Coframe", "Incidence", "Block",
	"Construct", "Dimension", "Vertex", "Vector", "Pierce", "Side",
	"Uniform", "Global", "Query", "Texture0", "Texture1", "Programs"};
const char *data[] = {"data","scalar","query"};
const char *name[] = {"setData","getUniform","putUniform","checkQuery",0};
void (*function[])(int, struct Update *) = {setData,getUniform,putUniform,checkQuery};
const char *program[] = {
    "Diplane", // Plane,Versor,Face -> display
    "Dipoint", // Point,Normal*3,Coordinate*3,Weight*3,Color*3,Tag*3,Frame -> display
    "Coplane", // Plane,Versor,Incidence -> Vertex
    "Copoint", // Point,Block -> Construct,Dimension
    "Adplane", // Plane,Versor,Face -> Side
    "Adpoint", // Point,Frame -> Side
    "Perplane", // Plane,Versor,Face -> Pierce
    "Perpoint", // Point,Frame -> Pierce
    "Replane", // Plane,Versor -> Vertex
    "Repoint", // Point -> Construct,Dimension
    "Explane", // Plane,Versor -> Vector
    "Expoint"}; // Point -> Vector
const char *space[] = {
	"Small", // specified space
	"Large"}; // cospace

Update *parseUpdate(const char *&ptr)
{
	Update init = {0}; Update *update = updates.get(); *update = init; int opt, i;
	if (sscanf(ptr," %d%n",&update->file,&opt) == 2) ptr += opt; else return 0;
	for (i = 0; i < Buffers; i++) {
		std::string str = " "; str += buffer[i]; str += "%n";
		if (sscanf(ptr,str.c_str(),&opt) == 1) {ptr += opt; update->buffer = (Buffer)i; break;}
	}
	if (i == Buffers) return 0;
	if (sscanf(ptr," %d%n",&update->offset,&opt) == 2) ptr += opt; else return 0;
	if (sscanf(ptr," %d%n",&update->size,&opt) == 2) ptr += opt; else return 0;
	int siz = (update->buffer == Texture0 || update->buffer == Texture1 ?
		update->width*update->height*3 + (4-(update->width*3)%4)*update->height : update->size);
	int pow = 0; while (siz>(1<<pow)) pow++; update->data = pools[pow].get();
	for (i = 0; i < siz; i++) {
		unsigned val;
		if (sscanf(ptr," %2x%n",&val,&opt) == 2) ptr += opt; else return 0;
		update->data[i] = val;
	}
	for (i = 0; name[i]; i++) {
		std::string str = " "; str += name[i]; str += "%n";
		if (sscanf(ptr,str.c_str(),&opt) == 1) {ptr += opt; update->function = function[i]; break;}
	}
	return update;
}

Render *parseRender(const char *&ptr)
{
	Render init = {0}; Render *render = renders.get(); *render = init; int opt, i;
	if (sscanf(ptr," %d%n",&render->file,&opt) == 2) ptr += opt; else return 0;
	for (i = 0; i < Programs; i++) {
		std::string str = " "; str += program[i]; str += "%n";
		if (sscanf(ptr,str.c_str(),&opt) == 1) {ptr += opt; render->program = (Program)i; break;}
	}
	if (i == Programs) return 0;
	for (i = 0; i < Spaces; i++) {
		std::string str = " "; str += space[i]; str += "%n";
		if (sscanf(ptr,str.c_str(),&opt) == 1) {ptr += opt; render->space = (Space)i; break;}
	}
	if (i == Spaces) return 0;
	if (sscanf(ptr," %d%n",&render->base,&opt) == 2) ptr += opt; else return 0;
	if (sscanf(ptr," %d%n",&render->count,&opt) == 2) ptr += opt; else return 0;
	if (sscanf(ptr," %d%n",&render->size,&opt) == 2) ptr += opt; else return 0;
	return render;
}

Command *parseCommand(const char *&ptr)
{
	Command init = {0}; Command *command = commands.get(); *command = init; int opt;
	if (sscanf(ptr," feedback%n",&opt) == 1) {ptr += opt; command->feedback = 1;}
	for (int i = 0; i < Fields; i++) {
		std::string str = " "; str += field[i]; str += "%n";
		while (sscanf(ptr,str.c_str(),&opt) == 1) {ptr += opt;
			Update *update = parseUpdate(ptr);
			if (!update) return 0;
			insert(command->update[(Field)i],update);
		}
	}
	while (sscanf(ptr," render%n",&opt) == 1) {ptr += opt;
		Render *render = parseRender(ptr);
		if (!render) return 0;
		insert(command->render,render);
	}
	if (sscanf(ptr," redraw%n",&opt) == 1) {ptr += opt;
		Command *redraw = parseCommand(ptr);
		if (!redraw) return 0;
		command->redraw = redraw;
	}
	if (sscanf(ptr," pierce%n",&opt) == 1) {ptr += opt;
		Command *pierce = parseCommand(ptr);
		if (!pierce) return 0;
		command->pierce = pierce;
	}
	return command;
}

void skip(const char *&ptr)
{
	if (ptr[0] == '-' && ptr[1] == '-') ptr += 2;
	while (ptr[0] && (ptr[0] != '-' || ptr[1] != '-')) ptr++;
}

void parse(std::string str, Message<Command*> &request)
{
	const char *ptr = str.c_str();
	while (*ptr) {int opt;
		if (sscanf(ptr,"--command%n",&opt) == 1) {ptr += opt;
			Command *command = parseCommand(ptr);
			if (!command) {skip(ptr); continue;}
			request.put(command);
		}
	}
}
