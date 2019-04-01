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

#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include "message.hpp"

class Read;
class Window;
class Polytope;
class System;
class Write;

class Script : public Thread
{
private:
	int nfile;
	Message<Data*> **rsp2read;
	Message<Data*> *rsp2window;
	Message<Data*> **req2polytope;
	Message<Data*> *rsp2system;
	Message<Command*> **req2command;
	Message<Data*> **req2write;
	Message<Data*> *req2system;
public:
	Message<Data*> read2req;
	Message<Data*> window2req;
	Message<Data*> polytope2rsp;
	Message<Data*> system2req;
	Message<Command*> command2rsp;
	Message<Data*> write2rsp;
	Message<Data*> system2rsp;
	Script(int n);
	void connect(int i, Read *ptr);
	void connect(Window *ptr);
	void connect(int i, Polytope *ptr);
	void connect(System *ptr);
	void connect(int i, Write *ptr);
	virtual void call();
};

#endif
