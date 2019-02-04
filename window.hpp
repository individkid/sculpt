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

enum Buffer {
	Plane, Versor, Point, Pierce,
	Face, Frame,
	Construct, Dimension, Vertex,
	Block, Incidence,
	Uniform, Texture,
	Buffers};
struct Handle
{
	GLenum target;
	GLuint handle;
};
enum Program {
    Diplane, Dipoint,
    Coplane, Copoint,
    Adplane, Adpoint,
    Perplane, Perpoint,
    Replane, Repoint,
    Programs};
struct Configure
{
	const char *vertex;
	const char *geometry;
	const char *fragment;
	GLuint handle;
	GLuint vao;
	GLenum mode;
	GLenum primitive;
	int feedback;
};
struct Subcmd
{
	enum Buffer buffer;
	int offset;
	int size;
	char *data;
};
struct Command
{
	Next<Subcmd> *allocs;
	Next<Subcmd> *writes;
	Next<Subcmd> *reads;
	enum Program program;
	int count;
	int size;
};

class Window : public Thread
{
private:
	GLFWwindow* window;
	static Handle handle[Buffers];
	static Configure configure[Programs];
	static int once;
	static GLuint compileShader(GLenum type, const char *source);
	static GLuint compileProgram(const char *vertex, const char *geometry, const char *fragment);
	static void configureDipoint();
	static void initOnce();
public:
	Message<Command> *request;
	Message<Command> *response;
	Window() : Thread(1), window(0) {}
	virtual void run();
	virtual void wake();
};

#endif
