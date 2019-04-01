/*
*    window.hpp start start opengl, wait for buffer changes and feedback requests
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

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "message.hpp"
#include "microcode.hpp"

class GLFWwindow;
class Object;
class Read;
class Write;
class Polytope;
class Script;

struct Queue
{
	Command *first;
	Command *last;
	Command *loop;
};

class Window : public Thread
{
private:
	GLFWwindow *window;
	int finish;
	int nfile;
	Object *object;
	Message<Data*> *req2script;
	Message<Command*> *rsp2script;
	Microcode microcode[Programs];
	Queue redraw;
	Queue pierce;
	Queue query;
	void allocBuffer(Update &update);
	void writeBuffer(Update &update);
	void bindBuffer(Update &update);
	void unbindBuffer(Update &update);
	void readBuffer(Update &update);
	void allocTexture2d(Update &update);
	void writeTexture2d(Update &update);
	void bindTexture2d(Update &update);
	void unbindTexture2d();
	void processResponse(Command &command);
	void processResponse(Data &data);
	void processData(Data &data);
	void processCommand(Command &command);
	void finishCommand(Command &command);
	void startCommand(Queue &queue, Command &command);
	void startQueue(Queue &queue);
	void finishQueue(Queue &queue);
	void swapQueue(Queue &queue, Command *&command);
public:
	Message<Command*> command2req;
	Message<Data*> read2req;
	Message<Data*> write2rsp;
	Message<Data*> polytope2rsp;
	Message<Command*> polytope2req;
	Message<Data*> script2rsp;
	Message<Command*> script2req;
	Window(int n);
	void connect(int i, Read *ptr);
	void connect(int i, Write *ptr);
	void connect(int i, Polytope *ptr);
	void connect(Script *ptr);
	void sendWrite(Data *data);
	void sendPolytope(Data *data);
	void sendScript(Data *data);
	void warpCursor(float *cursor);
	void maybeKill(int seq);
	char *lookup(int tagbits);
	virtual void init();
	virtual void call();
	virtual void wait();
	virtual void wake();
	virtual void done();
};

#endif
