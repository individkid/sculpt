/*
*    script.hpp thread for calculating metrics
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

#include "message.hpp"

class Polytope;
class System;
class Read;
class Write;
class Window;
struct lua_State;
typedef const char * (*lua_Reader) (lua_State *L, void *data, size_t *size);

class Script : public Thread
{
private:
	Message<Query> *rsp2window;
	Message<Query> *rsp2polytope;
	Message<Query> *rsp2system;
	Message<Query> **rsp2read;
	Message<State> **req2write;
	Message<Command> *req2window;
	Message<Data> *req2polytope;
	Message<Sound> *req2system;
public:
	Message<Query> window2req;
	Message<Query> polytope2req;
	Message<Query> system2req;
	Message<Query> read2req;
	Message<State> write2rsp;
	Message<Command> window2rsp;
	Message<Data> polytope2rsp;
	Message<Sound> system2rsp;
public:
	void connect(Polytope *ptr);
	void connect(System *ptr);
	void connect(int i, Read *ptr);
	void connect(int i, Write *ptr);
	void connect(Window *ptr);
private:
	virtual void init();
	virtual void call();
	virtual void done();
public:
	void execute(void *data, lua_Reader func);
	Script(int n, const char *path);
	virtual ~Script();
private:
	lua_State *state;
	int nfile; int cleanup;
private:
	void processQueries(Message<Query> &message);
	void processStates(Message<State> &message);
	void processCommands(Message<Command> &message);
	void processDatas(Message<Data> &message);
	void processSounds(Message<Sound> &message);
};
