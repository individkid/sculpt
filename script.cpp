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

#include "script.hpp"
#include "window.hpp"
#include "system.hpp"
#include "read.hpp"
#include "polytope.hpp"
#include "write.hpp"
#include <lua.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <stdio.h>

static Pool<Query> queries(__FILE__,__LINE__);
static Pool<State> states(__FILE__,__LINE__);
static Pool<Command> commands(__FILE__,__LINE__);
static Pool<Data> datas(__FILE__,__LINE__);
static Pool<Sound> sounds(__FILE__,__LINE__);

void Script::connect(Polytope *ptr)
{
    rsp2polytope = &ptr->script2rsp;
}

void Script::connect(System *ptr)
{
	rsp2system = &ptr->script2rsp;
}

void Script::connect(int i, Read *ptr)
{
	rsp2read[i] = &ptr->script2rsp;
}

void Script::connect(int i, Write *ptr)
{
	req2write[i] = &ptr->script2req;
}

void Script::connect(Window *ptr)
{
	rsp2window = &ptr->script2rsp;
	req2window = &ptr->script2req;
}

void Script::init()
{
	if (rsp2polytope == 0) error("unconnected rsp2polytope",0,__FILE__,__LINE__);
	if (rsp2system == 0) error("unconnected rsp2system",0,__FILE__,__LINE__);
	for (int i = 0; i < nfile; i++) if (rsp2read[i] == 0) error("unconnected rsp2read",i,__FILE__,__LINE__);
	for (int i = 0; i < nfile; i++) if (req2write[i] == 0) error("unconnected req2write",i,__FILE__,__LINE__);
	if (req2window == 0) error("unconnected req2window",0,__FILE__,__LINE__);
	if (req2polytope == 0) error("unconnected req2polytope",0,__FILE__,__LINE__);
	if (req2system == 0) error("unconnected req2system",0,__FILE__,__LINE__);
}

void Script::call()
{
	processQueries(window2req);
	processQueries(polytope2req);
	processQueries(system2req);
	processQueries(read2req);
	processStates(write2rsp);
	processCommands(window2rsp);
	processDatas(polytope2rsp);
	processSounds(system2rsp);
}

void Script::done()
{
	cleanup = 1;
	processQueries(polytope2req);
	processQueries(system2req);
	processQueries(read2req);
}

void Script::execute(void *data, lua_Reader func)
{
	if (lua_load(state,func,data,"script",0) == LUA_OK) {
	lua_pushlightuserdata(state,this);
	lua_call(state,1,0);} else {
	error(lua_tostring(state,-1),0,__FILE__,__LINE__);
	lua_pop(state,1);}
}

const char *filereader(lua_State *L, void *data, size_t *size)
{
	static int fd = 0;
	static char buffer[BUFFER_SIZE];
	if (fd == 0) fd = open((const char *)data,O_RDONLY);
	*size = read(fd,buffer,BUFFER_SIZE);
	if (*size == 0) {close(fd); fd = 0; return 0;}
	return buffer;
}

Script::Script(int n, const char *path) :
	rsp2read(new Message<Query>*[n]),
	req2write(new Message<State>*[n]),
	window2req(this,"Window->Query->Script"),
	polytope2req(this,"Polytope->Query->Script"),
	system2req(this,"System->Query->Script"),
	read2req(this,"Read->Query->Script"),
	write2rsp(this,"Script<-State<-Write"),
	window2rsp(this,"Script<-Command<-Window"),
	polytope2rsp(this,"Script<-Data<-Polytope"),
	system2rsp(this,"Script<-Sound<-System"),
	state(0), nfile(n), cleanup(0)
{
	state = luaL_newstate();
	char *argv;
	char buffer[strlen(path)+1];
	if (asprintf(&argv,"%s/sculpt.lux",dirname_r(path,buffer)) < 0) error("asprintf failed",errno,__FILE__,__LINE__);
	execute(argv,filereader);
}

Script::~Script()
{
	processStates(write2rsp);
	processCommands(window2rsp);
	processDatas(polytope2rsp);
	processSounds(system2rsp);
}

#define POP_ERROR(MESSAGE,LINE) { \
	message(MESSAGE,LINE,__FILE__,__LINE__); \
	lua_error(state);}

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

const char *reader(lua_State *L, void *data, size_t *size)
{
	static int done = 0;
	char *result = (char *)data;
	if (done) {done = 0; *size = 0; return 0;}
	done = 1; *size = strlen(result); return result;
}

void Script::processQueries(Message<Query> &message)
{
	Query *query; while (message.get(query)) {
	if (!cleanup) execute(query->script,reader);
	if (&message == &window2req) rsp2window->put(query);
	if (&message == &polytope2req) rsp2polytope->put(query);
	if (&message == &system2req) rsp2system->put(query);
	if (&message == &read2req) rsp2read[query->file]->put(query);}
}

void Script::processStates(Message<State> &message)
{
	State *state;
	while (message.get(state)) {/*put to Pools depending on state->change*/ states.put(state);}
}

void Script::processCommands(Message<Command> &message)
{
	Command *command;
	while (message.get(command)) {/*put to Pools depending on command->source*/ commands.put(command);}
}

void Script::processDatas(Message<Data> &message)
{
	Data *data;
	while (message.get(data)) {/*put to Pools depending on data->conf*/ datas.put(data);}
}

void Script::processSounds(Message<Sound> &message)
{
	Sound *sound;
	while (message.get(sound)) {/*put to Pools depending on sound->event*/ sounds.put(sound);}
}

