/*
*    parse.cpp function for converting string to Command
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

#include "parse.hpp"

extern "C" void getUniform(struct Update *update);
extern "C" void firstUniform(struct Update *update);
extern "C" void putUniform(struct Update *update);
extern "C" void checkQuery(struct Update *update);

const char *field[] = {"AllocField","WriteField","BindField","ReadField",0};
const char *buffer[] = {
	"Point0","Versor",
	"Point1","Normal","Coordinate","Weight","Color","Tag",
	"Point2",
	"Face1","Triple0","Triple1",
	"Pierce","Vector","Plane","Tagbits","Scalar",
	"Uniform","Query","Texture0","Texture1"};
const char *data[] = {"data","scalar","query"};
const char *name[] = {"getUniform","firstUniform","putUniform","checkQuery",0};
void (*function[])(struct Update *) = {getUniform,firstUniform,putUniform,checkQuery};
const char *program[] = {"Draw","Pierce","Sect0","Sect1","Side1","Side2"};

void Parse::get(const char *&ptr, Update *&update)
{
	Update init = {0}; update = updates.get(); *update = init; int len, i; const char *pat;
	len = number(ptr,update->file);
	if (len) ptr += len; else {updates.put(update); update = 0; return;}
	for (i = 0; i < Buffers; i++) {
		pat = cleanup(chars,concat(chars," ",buffer[i]));
		len = literal(ptr,pat);
		if (len) {ptr += len; update->buffer = (Buffer)i; break;}}
	if (i == Buffers) {updates.put(update); update = 0; return;}
	len = number(ptr,update->offset);
	if (len) ptr += len; else {updates.put(update); update = 0; return;}
	len = number(ptr,update->size);
	if (len) ptr += len; else {updates.put(update); update = 0; return;}
	int siz = (update->buffer == Texture0 || update->buffer == Texture1 ?
		update->width*update->height*3 + (4-(update->width*3)%4)*update->height :
		update->size);
	update->text = chars.get(siz);
	for (i = 0; i < siz; i++) {
		int val; len = number(ptr,val); update->text[i] = val;
		if (len) ptr += len; else {updates.put(update); update = 0; return;}}
	for (i = 0; name[i]; i++) {
		pat = cleanup(chars,concat(chars," ",name[i]));
		len = literal(ptr,pat);
		if (len) {ptr += len; update->function = function[i]; break;}}
}

void Parse::get(const char *&ptr, Render *&render)
{
	Render init = {0}; render = renders.get(); *render = init; int len, i; const char *pat;
	len = number(ptr,render->file);
	if (len) ptr += len; else {renders.put(render); render = 0; return;}
	for (i = 0; i < Programs; i++) {
		pat = cleanup(chars,concat(chars," ",program[i]));
		len = literal(ptr,pat);
		if (len) {ptr += len; render->program = (Program)i; break;}}
	if (i == Programs) {renders.put(render); render = 0; return;}
	len = number(pat,render->base);
	if (len) ptr += len; else {renders.put(render); render = 0; return;}
	len = number(pat,render->count);
	if (len) ptr += len; else {renders.put(render); render = 0; return;}
	len = number(pat,render->size);
	if (len) ptr += len; else {renders.put(render); render = 0; return;}
}

void Parse::get(const char *&ptr, int file, Command *&command)
{
	Command init = {0}; command = commands.get(); *command = init; int len; const char *pat;
	len = literal(ptr," feedback");
	if (len) {ptr += len; command->feedback = 1;}
	for (int i = 0; i < Fields; i++) {
		pat = cleanup(chars,concat(chars," ",field[i]));
		Update *update = 0; int inval = 0;
		while ((len = literal(ptr,pat))) {ptr += len;
			Update *temp; get(ptr,temp);
			if (!temp) {inval = 1; break;}
			insert(update,temp);}
		while (update) {
            insert(command->update[(Field)i],update);
            remove(update,update);}
        if (inval) {put(command); command = 0; return;}}
    Render *render = 0; int inval = 0;
	while ((len = literal(ptr," render"))) {ptr += len;
		Render *temp; get(ptr,temp);
		if (!temp) {inval = 1; break;}
		insert(render,temp);}
	while (render) {
		insert(command->render,render);
		remove(render,render);}
    if (inval) {put(command); command = 0; return;}
	if ((len = literal(ptr," redraw"))) {ptr += len;
		get(ptr,file,command->redraw);
		if (!command->redraw) {put(command); command = 0; return;}}
	if ((len = literal(ptr," pierce"))) {ptr += len;
		get(ptr,file,command->pierce);
		if (!command->pierce) {put(command); command = 0; return;}}
	Response *response = responses.get(); command->response = response;
	response->next = 0; response->file = file;
}

void Parse::get(const char *ptr, int file, Command *&command, Data *&window, Data *&polytope)
{
	int len; command = 0; window = 0; polytope = 0;
	len = len = literal(ptr,"--command");
	if (len) {ptr += len;
	get(ptr,file,command);
	if (command) return;}
	len = literal(ptr,"--test");
	if (len) {ptr += len;
	polytope = datas.get(); polytope->conf = TestConf; polytope->file = file;
	len = 0; while (ptr[len]) if (ptr[len] == '-' && ptr[len+1] == '-') break; else len++;
	polytope->text = prefix(chars,ptr,len+1); polytope->text[len] = 0;
	return;}
}

void Parse::put(Data *data)
{
	if (data->conf == TestConf) chars.put(strlen(data->text)+1,data->text);
	datas.put(data);
}

void Parse::put(Command *command)
{
	while (command) {
	for (int i = 0; i < Fields; i++) {
	Update *update = command->update[i];
	while (update) {updates.put(update); update = update->next;}}
	Render *render = command->render;
	while (render) {renders.put(render); render = render->next;}
	if (command->redraw) put(command->redraw);
	if (command->pierce) put(command->pierce);
	Response *response = command->response;
	while (response) {responses.put(response); response = response->next;}
	command = command->next;}
}
