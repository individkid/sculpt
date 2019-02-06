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
	Plane, Versor, Point, Normal, Coordinate, Weight, Color, Tag,
	Face, Frame, Incidence, Block,
	Construct, Dimension, Vertex, Vector, Pierce, Side,
	Uniform, Texture,
	Buffers};
struct Handle
{
	GLenum target;
	GLuint handle;
	GLenum usage;
};
enum Program {
    Diplane, // Plane,Versor,Face -> display
    Dipoint, // Point,Normal*3,Coordinate*3,Weight*3,Color*3,Tag*3,Frame -> display
    Coplane, // Plane,Versor,Incidence -> Vertex
    Copoint, // Point,Block -> Construct,Dimension
    Adplane, // Plane,Versor,Face -> Side
    Adpoint, // Point,Frame -> Side
    Perplane, // Plane,Versor,Face -> Pierce
    Perpoint, // Point,Frame -> Pierce
    Replane, // Plane,Versor -> Vertex
    Repoint, // Point -> Construct,Dimension
    Explane, // Plane,Versor -> Vector
    Expoint, // Point -> Vector
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
