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
class Write;
class Polytope;
class Read;

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
	int nfile; Object *object;
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
	void processData(std::string cmdstr);
	void startQueue(Queue &queue);
	void finishQueue(Queue &queue);
	void swapQueue(Queue &queue, Command *&command);
	void startCommand(Queue &queue, Command &command);
	void processCommand(Command &command);
	void finishCommand(Command &command);
public:
	Message<std::string> data; // get mode change and raw data from Read
	Message<Command*> request; // get Command from Polytope
	Window(int n);
	void connect(int i, Write *ptr);
	void connect(int i, Polytope *ptr);
	void connect(int i, Read *ptr);
	void sendData(Data *data);
	void warpCursor(float *cursor);
	virtual void call();
	virtual void wake();
};

#endif
