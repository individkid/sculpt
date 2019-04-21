/*
*    system.cpp thread for producing sound
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

#include <lua.hpp>
#include "script.hpp"
#include "window.hpp"
#include "system.hpp"
#include "read.hpp"
#include "polytope.hpp"
#include "write.hpp"

static Power<int> ints(__FILE__,__LINE__);
static Power<float> floats(__FILE__,__LINE__);
static Power<char> chars(__FILE__,__LINE__);
static Pool<Data> datas(__FILE__,__LINE__);
static Pool<Command> commands(__FILE__,__LINE__);
static Pool<Sound> sounds(__FILE__,__LINE__);

#define POP_ERROR(MESSAGE,LINE) { \
	message(MESSAGE,LINE,__FILE__,__LINE__); \
	datas.put(data); lua_error(state);}

#define POP_VALUE(RESULT,POP,TYPE,LINE) \
	if (lua_type(state,-1) != TYPE) POP_ERROR("bad script",LINE); \
	RESULT = POP(state,-1); lua_pop(state,1);

#define POP_TABLE(RESULT,POP,TYPE,LIMIT,LINE) \
	if (lua_type(state,-1) != LUA_TTABLE) POP_ERROR("bad script",LINE); \
	lua_pushnil(state); \
	for (int i = 0; i <= LIMIT; i++) { \
	if (lua_next(state,-2) == 0) { \
	if (i != LIMIT) POP_ERROR("extra table",LINE); \
	break;} \
	if (i == LIMIT) POP_ERROR("table extra",LINE); \
	POP_VALUE(RESULT[i],POP,TYPE,LINE);}

#define POP_STRING(RESULT,LINE) \
	if (lua_type(state,-1) != LUA_TSTRING) POP_ERROR("bad script",LINE); \
	temp = lua_tostring(state,-1); data->RESULT = chars.get(strlen(temp)+1); \
	strcpy(data->RESULT,temp); lua_pop(state,1);

#define POP_SCRIPT(RESULT,LINE) POP_VALUE(RESULT,(Script*)lua_touserdata,LUA_TUSERDATA,LINE)
#define POP_INT(RESULT,LINE) POP_VALUE(data->RESULT,lua_tointeger,LUA_TNUMBER,LINE)
#define POP_FLOAT(RESULT,LINE) POP_VALUE(data->RESULT,lua_tonumber,LUA_TNUMBER,LINE)
#define POP_CONF(RESULT,LINE) POP_VALUE(data->RESULT,(Configure)lua_tointeger,LUA_TNUMBER,LINE)
#define POP_INTS(RESULT,LIMIT,LINE) POP_TABLE(data->RESULT,lua_tointeger,LUA_TNUMBER,LIMIT,LINE)
#define POP_FLOATS(RESULT,LIMIT,LINE) POP_TABLE(data->RESULT,lua_tonumber,LUA_TNUMBER,LIMIT,LINE)

int req2polytope(lua_State *state)
{
	// TODO send Data from stack
	return 0;
}

int req2write(lua_State *state)
{
	Data *data = datas.get(); Script *self; const char *temp;
	POP_SCRIPT(self,__LINE__);
	POP_INT(file,__LINE__);
	POP_INT(plane,__LINE__);
	POP_CONF(conf,__LINE__);
	switch (data->conf) {
	case (AdditiveConf):
	case (SubtractiveConf):
	case (RefineConf):
	case (TweakConf):
	case (PerformConf):
	case (TransformConf):
	case (CylinderConf):
	case (ClockConf):
	case (NormalConf):
	case (ParallelConf):
	case (ScaleConf):
	case (RotateConf):
	case (TangentConf):
	case (TranslateConf):
	case (SessionConf):
	case (PolytopeConf):
	case (FacetConf):
	case (NumericConf):
	case (InvariantConf):
	case (SymbolicConf):
	case (RelativeConf):
	case (AbsoluteConf):
	break;
	case (IncludeConf):
	POP_STRING(text,__LINE__);
	break;
	case (ExcludeConf):
	break;
	case (PlaneConf):
	POP_INT(versor,__LINE__);
	data->vector = floats.get(3);
	POP_FLOATS(vector,3,__LINE__);
	break;
	case (MatrixConf):
	case (GlobalConf):
	data->matrix = floats.get(16);
	POP_FLOATS(matrix,16,__LINE__);
	break;
	case (SpaceConf):
	POP_INT(boundaries,__LINE__);
	POP_INT(regions,__LINE__);
	data->planes = ints.get(data->boundaries);
	POP_INTS(planes,data->boundaries,__LINE__);
	data->sides = ints.get(data->boundaries*data->regions);
	POP_INTS(sides,data->boundaries*data->regions,__LINE__);
	break;
	case (RegionConf):
	POP_INT(side,__LINE__);
	POP_INT(insides,__LINE__);
	POP_INT(outsides,__LINE__);
	data->inside = ints.get(data->insides);
	POP_INTS(inside,data->insides,__LINE__);
	data->outside = ints.get(data->outsides);
	POP_INTS(outside,data->outsides,__LINE__);
	break;
	case (InflateConf):
	break;
	case (PictureConf):
	POP_STRING(text,__LINE__);
	break;
	case (MetricConf):
	case (MacroConf):
	POP_FLOAT(delay,__LINE__);
	POP_INT(count,__LINE__);
	POP_INTS(ident,data->count,__LINE__);
	POP_STRING(script,__LINE__);
	break;
	case (ScriptConf):
	case (InvokeConf):
	POP_STRING(text,__LINE__);
	break;
	case (CommandConf):
	case (ConfigureConf):
	case (TimewheelConf):
	POP_ERROR("unimplemented",__LINE__);
	break;
	case (TestConf):
	POP_STRING(text,__LINE__);
	break;
	default: POP_ERROR("unimplemented",__LINE__);}
	self->req2write[data->file]->put(data);
	return 0;
}

const char *reader(lua_State *L, void *data, size_t *size)
{
	static int done = 0;
	char *result = (char *)data;
	if (done) {done = 0; *size = 0; return 0;}
	done = 1; *size = strlen(result); return result;
}

void Script::processCommands(Message<Command*> &message)
{
	Command *command; while (message.get(command)) {
		if (!cleanup) {
			if (&message == &window2rsp) {
				// TODO wite feedback, execute script at tagbits
			}
		}
		if (&message == &command2rsp) {
			// TODO deallocate field lists
			commands.put(command);
		}
		if (&message == &window2rsp) {
			// TODO deallocate field lists
			commands.put(command);
		}
	}
}

void Script::processSounds(Message<Sound*> &message)
{
	Sound *sound; while (message.get(sound)) {
		// TODO deallocate field lists
		sounds.put(sound);
	}
}

void Script::processDatas(Message<Data*> &message)
{
	Data *data; while (message.get(data)) {
		if (!cleanup) {
			if (&message == &read2req) {
				// execute script
				if (lua_load(state,reader,data->script,"read2req",0) == LUA_OK) {
				lua_pushlightuserdata(state,this);
				lua_call(state,1,0);} else {
				error(lua_tostring(state,-1),0,__FILE__,__LINE__);
				lua_pop(state,1);}}
			if (&message == &polytope2rsp) {
				// TODO write topology, and execute script
			}
			if (&message == &window2req) {
				// TODO write pierce plane file, execute script, and read result
			}
			if (&message == &system2rsp) {
				// TODO write stocks, and execute script
			}
			if (&message == &system2req) {
				// TODO write stocks, execute script, and read metric
			}
		}
		if (&message == &read2req) {
			rsp2read[data->file]->put(data);
		}
		if (&message == &polytope2rsp) {
			// TODO deallocate fields
			datas.put(data);
		}
		if (&message == &window2req) {
			rsp2window->put(data);
		}
		if (&message == &write2rsp) {
			// TODO deallocate fields
			datas.put(data);
		}
		if (&message == &system2rsp) {
			// TODO deallocate fields
			datas.put(data);
		}
	}
}

Script::Script(int n) : state(0), nfile(n), cleanup(0),
	rsp2read(new Message<Data*>*[n]), req2polytope(new Message<Data*>*[n]),
	req2command(new Message<Command*>*[n]), req2write(new Message<Data*>*[n]),
	req2sound(new Message<Sound*>*[n]), read2req(this,"Read->Data->Script"),
	polytope2rsp(this,"Script<-Data<-Polytope"), command2rsp(this,"Script<-Command<-Write"),
	write2rsp(this,"Script<-Data<-Write"), sound2rsp(this,"Script<-Sound<-Write"),
	system2rsp(this,"Script<-Data<-System"), system2req(this,"System->Data->Script"),
	window2req(this,"Window->Data->Script"), window2rsp(this,"Script<-Command<-Window")
{
}

Script::~Script()
{
	processDatas(polytope2rsp);
	processCommands(command2rsp);
	processDatas(write2rsp);
	processSounds(sound2rsp);
	processDatas(system2rsp);
	processCommands(window2rsp);
}

void Script::connect(int i, Read *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    rsp2read[i] = &ptr->script2rsp;
}

void Script::connect(int i, Polytope *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    req2polytope[i] = &ptr->script2req;
}

void Script::connect(int i, Write *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    req2command[i] = &ptr->command2req;
    req2write[i] = &ptr->script2req;
    req2sound[i] = &ptr->sound2req;
}

void Script::connect(System *ptr)
{
	rsp2system = &ptr->script2rsp;
	req2system = &ptr->script2req;
}

void Script::connect(Window *ptr)
{
	rsp2window = &ptr->script2rsp;
	req2window = &ptr->script2req;
}

void Script::init()
{
	state = luaL_newstate();
	lua_pushcfunction(state,::req2polytope);
	for (int i = 0; i < nfile; i++) if (rsp2read[i] == 0) error("unconnected rsp2read",i,__FILE__,__LINE__);
	for (int i = 0; i < nfile; i++) if (req2polytope[i] == 0) error("unconnected req2polytope",i,__FILE__,__LINE__);
	for (int i = 0; i < nfile; i++) if (req2command[i] == 0) error("unconnected req2command",i,__FILE__,__LINE__);
	for (int i = 0; i < nfile; i++) if (req2write[i] == 0) error("unconnected req2write",i,__FILE__,__LINE__);
	for (int i = 0; i < nfile; i++) if (req2sound[i] == 0) error("unconnected req2sound",i,__FILE__,__LINE__);
	if (rsp2system == 0) error("unconnected rsp2system",0,__FILE__,__LINE__);
	if (req2system == 0) error("unconnected req2system",0,__FILE__,__LINE__);
	if (req2window == 0) error("unconnected req2window",0,__FILE__,__LINE__);
}

void Script::call()
{
	processDatas(read2req);
	processDatas(polytope2rsp);
	processCommands(command2rsp);
	processDatas(write2rsp);
	processSounds(sound2rsp);
	processDatas(system2rsp);
	processDatas(system2req);
	processCommands(window2rsp);
}

void Script::done()
{
	cleanup = 1;
	processDatas(read2req);
	processDatas(system2req);
}
