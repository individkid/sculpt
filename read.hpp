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

#include "message.hpp"

class Window;
class Polytope;
class System;
class Script;
class File;

class Read : public Thread
{
private:
	Message<Query> *req2script;
public:
	Message<Query> script2rsp;
public:
	void connect(Script *ptr);
private:
	virtual void init();
	virtual void call();
	virtual void wait();
	virtual void done();
public:
	Read(int i, File *f);
	virtual ~Read();
private:
	int self;
	File *file;
	char *buffer;
	int length;
	int found;
};
