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

#include <iostream>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "message.hpp"

class Window : public Thread
{
private:
	GLFWwindow* window;
	GLuint compileShader(GLenum type, const char *source);
	GLuint compileProgram(const char *vertex, const char *geometry, const char *fragment);
public:
	Window() : Thread(true), window(0) {}
	virtual void run();
	virtual void wake();
};

#endif
