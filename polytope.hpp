/*
*    polytope.hpp thread for sample and classify
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

#ifndef POLYTOPE_HPP
#define POLYTOPE_HPP

#include "message.hpp"

class Polytope;
class Window;
class Read;
class Write;
class Script;

class Polytope : public Thread
{
private:
	Message<Data*> *rsp2read;
	Message<Data*> *rsp2window;
	Message<Data*> *req2write;
	Message<Command*> *req2window;
	Message<Data*> *rsp2script;
public:
	Message<Data*> read2req;
	Message<Data*> window2req;
	Message<Data*> write2rsp;
	Message<Command*> window2rsp;
	Message<Data*> script2req;
	Polytope(int i);
	void connect(Read *ptr);
	void connect(Window *ptr);
	void connect(Write *ptr);
	void connect(Script *ptr);
	virtual void call();
};

#endif
