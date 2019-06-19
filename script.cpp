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

void Script::connect(int i, Read *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    rsp2read[i] = &ptr->script2rsp;
}

void Script::connect(Polytope *ptr)
{
    req2polytope = &ptr->script2req;
}

void Script::connect(int i, Write *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    req2write[i] = &ptr->script2req;
}

void Script::connect(System *ptr)
{
	rsp2system = &ptr->script2rsp;
}

void Script::connect(Window *ptr)
{
	rsp2window = &ptr->script2rsp;
}

void Script::init()
{
	state = luaL_newstate();
	for (int i = 0; i < nfile; i++) if (rsp2read[i] == 0) error("unconnected rsp2read",i,__FILE__,__LINE__);
	if (req2polytope == 0) error("unconnected req2polytope",0,__FILE__,__LINE__);
	for (int i = 0; i < nfile; i++) if (req2write[i] == 0) error("unconnected req2write",i,__FILE__,__LINE__);
	if (rsp2system == 0) error("unconnected rsp2system",0,__FILE__,__LINE__);
	if (rsp2window == 0) error("unconnected rsp2window",0,__FILE__,__LINE__);
}

void Script::call()
{
	processDatas(read2req);
	processQueries(polytope2rsp);
	processStates(write2rsp);
	processSounds(system2req);
	processDatas(window2req);
}

void Script::done()
{
	cleanup = 1;
	processDatas(read2req);
	processSounds(system2req);
	processDatas(window2req);
}

Script::Script(int n) :
	rsp2read(new Message<Data>*[n]), req2write(new Message<State>*[n]),
	read2req(this,"Read->Data->Script"), polytope2rsp(this,"Script<-Data<-Polytope"),
	write2rsp(this,"Script<-Data<-Write"), system2req(this,"System->Data->Script"),
	window2req(this,"Window->Data->Script"), state(0), nfile(n), cleanup(0)
{
}

Script::~Script()
{
	processQueries(polytope2rsp);
	processStates(write2rsp);
}

static Pool<Query> queries(__FILE__,__LINE__);
static Power<char> chars(__FILE__,__LINE__);

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
		if (!cleanup) {
			// TODO execute script with response
		}
		if (&message == &polytope2rsp) {
			queries.put(query);}}
}

void Script::processStates(Message<State> &message)
{
    State *state; while (message.get(state)) {
        switch (state->change) {
            case (TextChange): chars.put(strlen(state->text)+1,state->text); break;
            case (GlobalChange):
            case (MatrixChange):
            case (PlaneChange):
            case (RegionChange):
            default: error("invalid change",state->change,__FILE__,__LINE__);}}
}

void Script::processSounds(Message<Sound> &message)
{
	Sound *sound; while (message.get(sound)) {
		if (!cleanup) {
			if (&message == &system2req) {
				// execute script
				if (lua_load(state,reader,sound->script,"system2req",0) == LUA_OK) {
				// TODO push sound parameters
				lua_pushlightuserdata(state,this);
				lua_call(state,1,0);} else {
				error(lua_tostring(state,-1),0,__FILE__,__LINE__);
				lua_pop(state,1);}}}
		if (&message == &system2req) {
			rsp2system->put(sound);}}
}

void Script::processDatas(Message<Data> &message)
{
	Data *data; while (message.get(data)) {
		if (!cleanup) {
			if (&message == &read2req) {
				// execute script
				if (lua_load(state,reader,data->script,"read2req",0) == LUA_OK) {
				lua_pushlightuserdata(state,this);
				lua_call(state,1,0);} else {
				error(lua_tostring(state,-1),0,__FILE__,__LINE__);
				lua_pop(state,1);}}}
		if (&message == &read2req) {
			rsp2read[data->file]->put(data);}}
}
