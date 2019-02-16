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

class Write;
class Polytope;
class Read;

enum Buffer {
	Plane, Versor, Point, Normal, Coordinate, Weight, Color, Tag,
	Face, Frame, Coface, Coframe, Incidence, Block,
	Construct, Dimension, Vertex, Vector, Pierce, Side,
	Uniform, Global, Texture0, Texture1, Buffers};
struct Handle
{
	GLenum target;
	GLenum unit;
	GLuint handle;
	GLenum usage;
	GLuint index;
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
	GLuint handle;
	GLenum mode;
	GLenum primitive;
};
struct Update
{
	enum Buffer buffer;
	int file;
	union {int offset; int width;};
	union {int size; int height;};
	GLuint handle;
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
	int feedback;
	Next<Update> *binds;
	Next<Update> *reads;
	Next<Render> *renders;
	Message<Command> *response;
	Command *command;
};
struct Action
{
};
struct File
{
	Handle handle[Buffers];
	GLuint vao[Programs][Spaces];

};

class Window : public Thread
{
private:
	Write **write; // send raw data to Write
	Polytope **polytope; // send Action to Polytope
	Read **read; // for completeness
	GLFWwindow* window;
	int nfile; File *file;
	Configure configure[Programs];
	void initProgram(Program program);
	void initDipoint();
	void initConfigure(const char *vertex, const char *geometry, const char *fragment, int count, const char **feedback, Configure &program);
	void initShader(GLenum type, const char *source, GLuint &handle);
	void initFile(File &file);
	void initHandle(enum Buffer buffer, int first, Handle &handle);
	void initVao(enum Buffer buffer, enum Program program, enum Space space, GLuint vao, GLuint handle);
	void initVao3f(GLuint index, GLuint handle);
	void initVao2f(GLuint index, GLuint handle);
	void initVao4u(GLuint index, GLuint handle);
	void initVao2u(GLuint index, GLuint handle);
	void allocBuffer(Update &update);
	void writeBuffer(Update &update);
	void bindBuffer(Update &update);
	void unbindBuffer(Update &update);
	void readBuffer(Update &update);
	void allocTexture2d(Update &update);
	void writeTexture2d(Update &update);
	void bindTexture2d(Update &update);
	void unbindTexture2d();
public:
	Message<char[STRING_ARRAY_SIZE]> mode; // get -- from Read
	Message<char[STRING_ARRAY_SIZE]> data; // get raw data from Read
	Message<Command> request; // get Command from Polytope
	Window(int n) : Thread(1), write(new Write *[n]), polytope(new Polytope *[n]), read(new Read *[n]), window(0), nfile(n), file(new File[n]), data(this), request(this) {}
	void connect(int i, Write *ptr) {if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__); write[i] = ptr;}
	void connect(int i, Polytope *ptr) {if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__); polytope[i] = ptr;}
	void connect(int i, Read *ptr) {if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__); read[i] = ptr;}
	virtual void call();
	virtual void wake();
};

#endif
