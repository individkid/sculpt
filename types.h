/*
*    types.h c type structs
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

#ifndef TYPES_HPP
#define TYPES_HPP

#define INFINITE 1000000

struct GLFWwindow;

typedef unsigned MYenum;
typedef unsigned MYuint;
enum Buffer {
	Plane, Versor, Point, Normal, Coordinate, Weight, Color, Tag,
	Face, Frame, Coface, Coframe, Incidence, Block,
	Construct, Dimension, Vertex, Vector, Pierce, Side,
	Uniform, Global, Texture0, Texture1, Buffers};
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
enum Space {
	Small, // specified space
	Large, // cospace
	Spaces};
struct Format
{
// assume sizeof(float) is multiple of 4
float affine[16];
float perplane[16];
MYuint tagplane;
char filler1[4-sizeof(MYuint)]; MYuint taggraph;
char filler2[4-sizeof(MYuint)]; float cutoff;
float slope;
float aspect;
};
struct Update
{
	struct Update *next;
	enum Buffer buffer;
	union {int offset; int width;};
	union {int size; int height;};
	MYuint handle;
	union {char *data; struct Format *format;};
	void (*function)(int, struct Update *);
};
struct Render
{
	struct Render *next;
	enum Program program;
	enum Space space;
	int base;
	int count;
	int size;
};
struct Command
{
	int file;
	struct Update *allocs;
	struct Update *writes;
	int feedback;
	struct Update *binds;
	struct Update *reads;
	struct Render *renders;
	int response;
	struct Command *command;
};
struct Action
{
	int file;
};
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

#endif
