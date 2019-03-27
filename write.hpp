/*
*    write.hpp thread for writing to pipe
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

#ifndef WRITE_HPP
#define WRITE_HPP

#include "message.hpp"
#include "parse.hpp"

class Window;
class Polytope;
class Script;

class Write : public Thread
{
private:
	const char *name;
	int pipe;
	// Window->Data->Write
	Message<Data*> *rsp2data2window;
	// Polytope->Data->Write
	Message<Data*> *rsp2data2polytope;
	// Script->Data->Write
	Message<Data*> *rsp2data2script;
	Parse parse;
public:
	Message<Data*> window2data2req;
	Message<Data*> polytope2data2req;
	Message<Data*> script2data2req;
	Write(int i, const char *n);
	void connect(Window *ptr);
	void connect(Polytope *ptr);
	void connect(Script *ptr);
	virtual void init();
	virtual void call();
	virtual void done();
	void process(Message<Data*> &req, Message<Data*> &rsp);
};

#endif
