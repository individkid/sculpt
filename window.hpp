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

class Window : public Thread
{
private:
	GLFWwindow *window;
	int nfile; Object *object;
	Microcode microcode[Programs];
	Command redrawCommand;
	Command pierceCommand;

	void allocBuffer(int file, Update &update);
	void writeBuffer(int file, Update &update);
	void bindBuffer(int file, Update &update);
	void unbindBuffer(int file, Update &update);
	void readBuffer(int file, Update &update);
	void allocTexture2d(int file, Update &update);
	void writeTexture2d(int file, Update &update);
	void bindTexture2d(int file, Update &update);
	void unbindTexture2d();
public:
	Message<std::string> data; // get mode change and raw data from Read
	Message<Command> request; // get Command from Polytope
	Message<Command> response; // send Command back to Polytope
	Window(int n);
	void connect(int i, Write *ptr);
	void connect(int i, Polytope *ptr);
	void connect(int i, Read *ptr);
	virtual void call();
	virtual void wake();
	void processData(std::string cmdstr);
	void processCommand(Command &command);
};

#endif
