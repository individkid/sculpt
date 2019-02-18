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

class Write;
class Polytope;
class Read;
class GLFWwindow;

enum ClickMode {
	AdditiveMode,
	SubtractiveMode,
	RefineMode,
	TransformMode,
	RevealMode,
	HideMode,
	TweakMode,
	RandomizeMode,
	ClickModes};
enum MouseMode {
	RotateMode,
	LookMode,
	TangentMode,
	TranslateMode,
	MouseModes};
enum RollerMode {
	CylinderMode,
	ClockMode,
	NormalMode,
	ScaleMode,
	DriveMode,
	RollerModes};
enum TargetMode {
	SessionMode,
	PolytopeMode,
	FacetMode,
	TargetModes};
enum FixedMode {
	NumericMode,
	InvariantMode,
	SymbolicMode,
	FixedModes};
struct Handle
{
	MYenum target;
	MYenum unit;
	MYuint handle;
	MYenum usage;
	MYuint index;
};
struct Configure
{
	MYuint handle;
	MYenum mode;
	MYenum primitive;
};
struct File
{
	Handle handle[Buffers];
	MYuint vao[Programs][Spaces];

};

class Window : public Thread
{
private:
	Write **write; // send raw data to Write
	Polytope **polytope; // send Action to Polytope
	Read **read; // for completeness
	GLFWwindow *window;
	int nfile; File *file;
	Configure configure[Programs];
	void initProgram(Program program);
	void initDipoint();
	void initConfigure(const char *vertex, const char *geometry, const char *fragment, int count, const char **feedback, Configure &program);
	void initShader(MYenum type, const char *source, MYuint &handle);
	void initFile(File &file);
	void initHandle(enum Buffer buffer, int first, Handle &handle);
	void initVao(enum Buffer buffer, enum Program program, enum Space space, MYuint vao, MYuint handle);
	void initVao3f(MYuint index, MYuint handle);
	void initVao2f(MYuint index, MYuint handle);
	void initVao4u(MYuint index, MYuint handle);
	void initVao2u(MYuint index, MYuint handle);
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
	Message<std::string> data; // get mode change and raw data from Read
	Message<Command> request; // get Command from Polytope
	Window(int n);
	void connect(int i, Write *ptr) {if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__); write[i] = ptr;}
	void connect(int i, Polytope *ptr) {if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__); polytope[i] = ptr;}
	void connect(int i, Read *ptr) {if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__); read[i] = ptr;}
	virtual void call();
	virtual void wake();
	int processData(std::string cmdstr);
	void processCommand(Command &command);
};

#endif
