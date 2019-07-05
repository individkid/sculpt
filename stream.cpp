/*
*    stream.cpp functions for converting pipe bytes to Command
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

#include <unistd.h>
#include "stream.hpp"
extern "C" {
#include "haskell.h"
}

static Pool<Command> commands(__FILE__,__LINE__);
static Pool<Update> updates(__FILE__,__LINE__);
static Pool<Render> renders(__FILE__,__LINE__);
static Pool<Query> queries(__FILE__,__LINE__);
static Pool<State> states(__FILE__,__LINE__);
static Pool<Data> datas(__FILE__,__LINE__);
static Power<float> floats(__FILE__,__LINE__);
static Power<char> chars(__FILE__,__LINE__);
static Power<int> ints(__FILE__,__LINE__);

void Stream::get(int fd, Command *&command)
{
	// TODO
}

void Stream::get(int fd, Update *&update)
{
	// TODO
}

void Stream::get(int fd, Render *&render)
{
	// TODO
}

void Stream::get(int fd, Query *&query)
{
	// TODO
}

void Stream::get(int fd, State *&state)
{
	// TODO
}

void Stream::get(int fd, Data *&data)
{
	data = datas.get();
	exOpcode(fd,FileOp); data->file = rdInt(fd);
	exOpcode(fd,PlaneOp); data->plane = rdInt(fd);
	exOpcode(fd,ConfOp); data->conf = (Configure)rdConfigure(fd);
	switch (data->conf) {
	default: error("unimplemented configure",data->conf,__FILE__,__LINE__);}
}

Opcode Stream::get(int fd, Data *&data, State *&state,
	Query *&query, Command *&command)
{
	data = 0; query = 0; command = 0;
	Opcode opcode = (Opcode)rdOpcode(fd);
	switch (opcode) {
	// TODO for responses with data read into the struct pointed to by rdPointer
	case (ReadOp): data = (struct Data *)rdPointer(fd); break;
	case (WriteOp): get(fd,state); break;
	case (QueryOp): query = (struct Query *)rdPointer(fd); /*TODO fill query with response*/ break;
	case (CommandOp): get(fd,command); break;
	default: error("unimplemented opcode",opcode,__FILE__,__LINE__);}
	return opcode;
}

void Stream::put(int fd, Opcode opcode, Command *command)
{
	// TODO
}

void Stream::put(int fd, Opcode opcode, Query *query)
{
	wrOpcode(fd,opcode); wrPointer(fd,query);
	// TODO write query arguments
}

void Stream::put(int fd, Opcode opcode, State *state)
{
	// TODO
}

void Stream::put(int fd, Opcode opcode, Data *data)
{
	int rsp; switch (opcode) {
	case (ReadOp): rsp = 0; break;
	case (WriteOp): rsp = 1; break;
	default: error("unimplemented opcode",opcode,__FILE__,__LINE__);}
	wrOpcode(fd,opcode); wrPointer(fd,(rsp?0:data));
	wrOpcode(fd,FileOp); wrInt(fd,data->file);
	wrOpcode(fd,PlaneOp); wrInt(fd,data->plane);
	wrOpcode(fd,ConfOp); wrConfigure(fd,data->conf);
	switch (data->conf) {
	default: error("unimplemented configure",data->conf,__FILE__,__LINE__);}
	if (rsp) {switch (data->conf) {
	default: error("unimplemented configure",data->conf,__FILE__,__LINE__);}
	datas.put(data);}
}

void Stream::put(Command *command)
{
	while (command) {
	switch (command->source) {
	case (ConfigureSrc): break;
	case (ModeSrc): break;
	case (MatrixSrc): floats.put(16,command->matrix); break;
	case (GlobalSrc): floats.put(16,command->matrix); break;
	case (PolytopeSrc): {
	for (int i = 0; i < Fields; i++) {
	Update *update = command->update[i];
	while (update) {updates.put(update); update = update->next;}}
	Render *render = command->render;
	while (render) {renders.put(render); render = render->next;}
	if (command->redraw) put(command->redraw);
	if (command->pierce) put(command->pierce);
	break;}
	case (MacroSrc): chars.put(strlen(command->script)+1,command->script); break;
	case (HotkeySrc): chars.put(strlen(command->script)+1,command->script); break;
	default: error("invalid source",command->source,__FILE__,__LINE__);}
	Command *temp = command; command = command->next; commands.put(temp);}
}

void Stream::put(Query *query)
{
	while (query) {
	Query *temp = query; query = query->next; queries.put(temp);}
}

void Stream::put(State *state)
{
	while (state) {
	switch (state->change) {
	case (SculptChange): break;
	case (GlobalChange): floats.put(16,state->matrix); break;
	case (MatrixChange): floats.put(16,state->matrix); break;
	case (PlaneChange): floats.put(3,state->vector); break;
	case (RegionChange):
	ints.put(state->insides,state->inside);
	ints.put(state->outsides,state->outside);
	break;
	case (TextChange): chars.put(strlen(state->text)+1,state->text); break;
	default: error("invalid change",state->change,__FILE__,__LINE__);}
	State *temp = state; state = state->next; states.put(temp);}
}

void Stream::put(Data *data)
{
	while (data) {
	switch (data->conf) {
	case (InflateConf): break;
	case (SpaceConf): ints.put(data->boundaries,data->planes);
	ints.put(data->boundaries*data->regions,data->sides); break;
	case (RegionConf): ints.put(data->insides,data->inside); 
	ints.put(data->outsides,data->outside); break;
	case (PlaneConf): floats.put(3,data->vector); break;
	case (PictureConf): chars.put(strlen(data->filename)+1,data->filename); break;
	case (PressConf): break;
	case (RelativeConf): floats.put(3,data->fixed); break;
	case (AbsoluteConf): break;
	case (RefineConf): floats.put(3,data->pierce); break;
	case (ManipConf): floats.put(16,data->matrix); break;
	case (ClickConf): break;
	case (AdditiveConf): break;
	case (SubtractiveConf): break;
	default: error("invalid conf",data->conf,__FILE__,__LINE__);}
	Data *temp = data; data = data->next; datas.put(data);}
}

