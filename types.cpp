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
#include "types.h"
#include "message.hpp"

extern "C" void getUniform(struct Update *update);
extern "C" void putUniform(struct Update *update);
extern "C" void checkQuery(struct Update *update);

static Pool<Command> commands;
static Pool<Update> updates;
static Pool<Render> renders;
static Pool<Response> responses;
static Power<char> chars;
static Pool<Data> datas;

const char *field[] = {"AllocField","WriteField","BindField","ReadField",0};
const char *buffer[] = {
	"Plane", "Versor", "Point", "Normal", "Coordinate", "Weight", "Color", "Tag",
	"Face", "Frame", "Coface", "Coframe", "Incidence", "Block",
	"Construct", "Dimension", "Vertex", "Vector", "Pierce", "Side",
	"Uniform", "Global", "Query", "Texture0", "Texture1", "Programs"};
const char *data[] = {"data","scalar","query"};
const char *name[] = {"getUniform","putUniform","checkQuery",0};
void (*function[])(struct Update *) = {getUniform,putUniform,checkQuery};
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
	Update init = {0}; Update *update = updates.get(); *update = init; int len, i; const char *pat;
	len = number(ptr,update->file);
	if (len) ptr += len; else return 0;
	for (i = 0; i < Buffers; i++) {
		pat = cleanup(chars,concat(chars," ",buffer[i]));
		len = literal(ptr,pat);
		if (len) {ptr += len; update->buffer = (Buffer)i; break;}}
	if (i == Buffers) return 0;
	len = number(ptr,update->offset);
	if (len) ptr += len; else return 0;
	len = number(ptr,update->size);
	if (len) ptr += len; else return 0;
	int siz = (update->buffer == Texture0 || update->buffer == Texture1 ?
		update->width*update->height*3 + (4-(update->width*3)%4)*update->height :
		update->size);
	update->text = chars.get(siz);
	for (i = 0; i < siz; i++) {
		int val; len = number(ptr,val); update->text[i] = val;
		if (len) ptr += len; else return 0;}
	for (i = 0; name[i]; i++) {
		pat = cleanup(chars,concat(chars," ",name[i]));
		len = literal(ptr,pat);
		if (len) {ptr += len; update->function = function[i]; break;}}
	return update;
}

Render *parseRender(const char *&ptr)
{
	Render init = {0}; Render *render = renders.get(); *render = init; int len, i; const char *pat;
	len = number(ptr,render->file);
	if (len) ptr += len; else return 0;
	for (i = 0; i < Programs; i++) {
		pat = cleanup(chars,concat(chars," ",program[i]));
		len = literal(ptr,pat);
		if (len) {ptr += len; render->program = (Program)i; break;}}
	if (i == Programs) return 0;
	for (i = 0; i < Spaces; i++) {
		pat = cleanup(chars,concat(chars," ",space[i]));
		len = literal(ptr,pat);
		if (len) {ptr += len; render->space = (Space)i; break;}}
	if (i == Spaces) return 0;
	len = number(pat,render->base);
	if (len) ptr += len; else return 0;
	len = number(pat,render->count);
	if (len) ptr += len; else return 0;
	len = number(pat,render->size);
	if (len) ptr += len; else return 0;
	return render;
}

Command *parseCommand(const char *&ptr)
{
	Command init = {0}; Command *command = commands.get(); *command = init; int len; const char *pat;
	len = literal(ptr," feedback");
	if (len) {ptr += len; command->feedback = 1;}
	for (int i = 0; i < Fields; i++) {
		pat = cleanup(chars,concat(chars," ",field[i]));
		Update *update = 0;
		while ((len = literal(ptr,pat))) {ptr += len;
			insert(update,parseUpdate(ptr));
			if (!update) return 0;}
		while (update) {
            insert(command->update[(Field)i],update);
            remove(update,update);}}
    Render *render = 0;
	while ((len = literal(ptr," render"))) {ptr += len;
		insert(render,parseRender(ptr));
		if (!render) return 0;}
	while (render) {
		insert(command->render,render);
		remove(render,render);}
	if ((len = literal(ptr," redraw"))) {ptr += len;
		command->redraw = parseCommand(ptr);
		if (!command->redraw) return 0;}
	if ((len = literal(ptr," pierce"))) {ptr += len;
		command->pierce = parseCommand(ptr);
		if (!command->pierce) return 0;}
	return command;
}

int parse(const char *ptr, Command *&command, Data *&data, ThreadType &dest, ThreadType source, int file)
{
	int len;
	len = literal(ptr,"--command");
	if (len) {ptr += len;
	command = parseCommand(ptr);
	if (command) {
	Response *response = responses.get(); command->response = response;
	response->next = 0; response->file = file; response->thread = source;
	data = 0; return 1;}}
	len = literal(ptr,"--test");
	if (len) {ptr += len;
	data = datas.get(); data->thread = ReadType; data->conf = TestConf; data->file = file;
	len = 0; while (ptr[len]) if (ptr[len] == '-' && ptr[len+1] == '-') break; else len++;
	data->text = prefix(chars,ptr,len+1); data->text[len] = 0;
	dest = PolytopeType; command = 0; return 1;}
	return 0;
}

void unparseCommand(Command *command)
{
}

void unparseData(Data *data)
{
	if (data->conf == TestConf) chars.put(strlen(data->text)+1,data->text);
	datas.put(data);
}
