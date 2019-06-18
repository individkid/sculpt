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

#include "message.hpp"

class Window;
class Polytope;
class Script;
class File;

class Write : public Thread
{
private:
	Message<State> *rsp2polytope;
	Message<State> *rsp2script;
	Message<State> *rsp2window;
public:
	Message<State> polytope2req;
	Message<State> script2req;
	Message<State> window2req;
public:
	void connect(Polytope *ptr);
	void connect(Script *ptr);
	void connect(Window *ptr);
private:
	virtual void init();
	virtual void call();
	virtual void done();
public:
	Write(int i, File *f);
private:
	int self;
	File *file;
};
