/*
*    read.hpp thread for reading and appending file
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

#ifndef READ_HPP
#define READ_HPP

#include "message.hpp"

class Window;
class Polytope;
class System;
class Script;

	// Read->Command->Window
	// Read->Data->Window
	// Read->Data->Polytope
	// Read->Data->System
	// Read->Data->Script
class Read : public Thread
{
private:
	const char *name;
	int file;
	int pipe;
	int self;
	off_t fpos;
	Message<Command*> *req2command2window;
	Message<Data*> *req2data2window;
	Message<Data*> *req2data2polytope;
	Message<Data*> *req2data2system;
	Message<Data*> *req2data2script;
	Message<Data*> *thread2data2rsp[5];
	void parse(const char *ptr, int file);
	void unparse(Data *data);
	void unparse(Command *command);
public:
	Message<Command*> window2command2rsp;
	Message<Data*> window2data2rsp;
	Message<Data*> polytope2data2rsp;
	Message<Data*> system2data2rsp;
	Message<Data*> script2data2rsp;
	Read(int i, const char *n);
	void connect(Window *ptr);
	void connect(Polytope *ptr);
	void connect(System *ptr);
	void connect(Script *ptr);
	virtual void init();
	virtual void call();
	virtual void wait();
	virtual void done();
};

#endif
