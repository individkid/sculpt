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

struct Queues
{
	Queue redraw;
	Queue pierce;
	Queue query;
};

class Window : public Thread
{
private:
	GLFWwindow *window;
	int finish;
	int nfile;
	Queues command;
	Queues polytope;
	Queues script;
	Object *object;
	Microcode microcode[Programs];
	Message<Data*> *req2polytope;
	Message<Command*> *rsp2script;
	Message<Data*> *req2script;
	void allocBuffer(Update &update);
	void writeBuffer(Update &update);
	void bindBuffer(Update &update);
	void unbindBuffer(Update &update);
	void readBuffer(Update &update);
	void allocTexture2d(Update &update);
	void writeTexture2d(Update &update);
	void bindTexture2d(Update &update);
	void unbindTexture2d();
	void swapQueue(Queue &queue, Command *&command);
	void startCommand(Command &command);
	void finishCommand(Command &command, Queues &queues);
	void processQueues(Queues &queues);
	void processQueue(Queue &queue, Queues &queues);
	void processCommands(Message<Command*> &message, Queues &queues);
	void processDatas(Message<Data*> &message);
public:
	Message<Command*> command2req;
	Message<Data*> read2req;
	Message<Data*> polytope2rsp;
	Message<Command*> polytope2req;
	Message<Data*> write2rsp;
	Message<Command*> script2req;
	Message<Data*> script2rsp;
	Window(int n);
	virtual ~Window();
	void connect(int i, Read *ptr);
	void connect(int i, Write *ptr);
	void connect(Polytope *ptr);
	void connect(Script *ptr);
	void sendWrite(Data *data);
	void sendPolytope(Data *data);
	void sendScript(Data *data);
	void warpCursor(float *cursor);
	void maybeKill(int seq);
private:
	virtual void init();
	virtual void call();
	virtual void wait();
	virtual void wake();
	virtual void done();
};
