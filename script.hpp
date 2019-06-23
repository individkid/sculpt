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

class Read;
class Window;
class Polytope;
class System;
class Write;
struct lua_State;

class Script : public Thread
{
private:
	Message<Query> **rsp2read;
	Message<Data> *req2polytope;
	Message<Query> *rsp2polytope;
	Message<State> **req2write;
	Message<Query> *rsp2system;
public:
	Message<Query> read2req;
	Message<Data> polytope2rsp;
	Message<Query> polytope2req;
	Message<State> write2rsp;
	Message<Query> system2req;
public:
	void connect(int i, Read *ptr);
	void connect(Polytope *ptr);
	void connect(int i, Write *ptr);
	void connect(System *ptr);
private:
	virtual void init();
	virtual void call();
	virtual void done();
public:
	Script(int n);
	virtual ~Script();
private:
	lua_State *state;
	int nfile; int cleanup;
private:
	void processQueries(Message<Query> &message);
	void processStates(Message<State> &message);
	void processSounds(Message<Sound> &message);
	void processDatas(Message<Data> &message);
};
