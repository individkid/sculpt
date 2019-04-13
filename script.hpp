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

class Script : public Thread
{
private:
	int nfile; int cleanup;
	Message<Data*> **rsp2read;
	Message<Data*> **rsp2polytope;
	Message<Data*> **req2polytope;
	Message<Command*> **req2command;
	Message<Data*> **req2write;
	Message<Data*> *rsp2system;
	Message<Data*> *req2system;
	Message<Command*> *req2window;
	void processCommands(Message<Command*> &message);
	void processDatas(Message<Data*> &message);
public:
	Message<Data*> read2req;
	Message<Data*> polytope2rsp;
	Message<Data*> polytope2req;
	Message<Command*> command2rsp;
	Message<Data*> write2rsp;
	Message<Data*> system2rsp;
	Message<Data*> system2req;
	Message<Command*> window2rsp;
	Script(int n);
	virtual ~Script();
	void connect(int i, Read *ptr);
	void connect(int i, Polytope *ptr);
	void connect(int i, Write *ptr);
	void connect(System *ptr);
	void connect(Window *ptr);
private:
	virtual void init();
	virtual void call();
	virtual void done();
};
