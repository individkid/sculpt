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

#include "message.hpp"

class Read;
class Write;
class Script;
class Window;

struct Plane
{
	int versor;
	float vector[3];
	int tagbits;
};

class Polytope : public Thread
{
private:
	int nfile;
	int hs2read[2], read2hs[2];
	int hs2data[2], data2hs[2];
	int hs2script[2], script2hs[2];
	int hs2window[2], window2hs[2];
	int hs2command[2], command2hs[2];
	int hs2open;
	int open2hs;
	Message<Data*> **rsp2read;
	Message<Data*> **req2write;
	Message<Data*> *rsp2script;
	Message<Data*> *rsp2window;
	Message<Command*> *req2window;
	void close(int hs2close, int close2hs);
	void processCommands(Message<Command*> &message);
	void processDatas(Message<Data*> &message);
public:
	Message<Data*> read2req;
	Message<Data*> write2rsp;
	Message<Data*> script2req;
	Message<Command*> window2rsp;
	Message<Data*> window2req;
	Polytope(int n, const char *path);
	virtual ~Polytope();
	void connect(int i, Read *ptr);
	void connect(int i, Write *ptr);
	void connect(Script *ptr);
	void connect(Window *ptr);
private:
	virtual void init();
	virtual void call();
	virtual void done();
};
