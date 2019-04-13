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

class Read : public Thread
{
private:
	const char *name;
	int file;
	int pipe;
	int self;
	off_t fpos, mpos, gpos;
	int mlen, glen, mnum, gnum;
	int livelock;
	Message<Command*> *req2command;
	Message<Data*> *req2window;
	Message<Data*> *req2polytope;
	Message<Data*> *req2system;
	Message<Data*> *req2script;
public:
	Message<Command*> command2rsp;
	Message<Data*> window2rsp;
	Message<Data*> polytope2rsp;
	Message<Data*> system2rsp;
	Message<Data*> script2rsp;
	Read(int i, const char *n);
	virtual ~Read();
	void connect(Window *ptr);
	void connect(Polytope *ptr);
	void connect(System *ptr);
	void connect(Script *ptr);
private:
	virtual void init();
	virtual void call();
	virtual void wait();
	virtual void done();
	char *read();
	char *split(char *&str);
	void sync(const char *str, const char *pat, off_t pos, off_t &sav, int &len, int &num, enum Configure conf);
	int trywrlck();
	int race();
	void unwrlck();
	int intr();
	int check();
	int read(char *&str);
	int sync(const char *str, const char *pat, off_t pos, int len, int &num);
	void write(const char *str);
	void backoff();
	int getrdlck();
	void unrdlck();
};
