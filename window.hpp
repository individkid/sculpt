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
	Face, Frame, Coface, Coframe, Incidence, Block,
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
    Diplane, // Plane,Versor,Coface -> display
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
	GLuint handle;
	GLenum mode;
	GLenum primitive;
	int feedback;
};
struct Update
{
	enum Buffer buffer;
	int file;
	int offset;
	int size;
	char *data;
};
enum Space {
	Small, // specified space
	Large, // cospace
	Spaces};
struct Render
{
	enum Program program;
	enum Space space;
	int base;
	int count;
	int size;
	int file;
};
struct Command
{
	Next<Update> *allocs;
	Next<Update> *writes;
	Next<Update> *reads;
	Next<Render> *renders;
	Message<Command> *response;
};
struct File
{
	Handle handle[Buffers];
	GLuint vao[Programs][Spaces];

};

class Window : public Thread
{
private:
	GLFWwindow* window;
	File *file;
	int argc;
	static Configure configure[Programs];
	static GLuint compileShader(GLenum type, const char *source);
	static void configureDipoint();
	void initVao(File *file, enum Program program, enum Buffer buffer);
	void initBuffer(Handle *handle);
public:
	Message<Command> *request;
	Window(int argc, char *argv[]) : Thread(1), window(0) {}
	virtual void run();
	virtual void wake();
};

#endif
