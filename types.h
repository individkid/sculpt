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

#ifndef TYPES_H
#define TYPES_H

#define INFINITE 1000000
#define BASE 1.1
#define DELAY 0.01
#define ANGLE (1.0/30.0)
#define LENGTH (1.0/30.0)

struct GLFWwindow;

typedef unsigned MYenum;
typedef unsigned MYuint;
enum Buffer {
	Plane, Versor, Point, Normal, Coordinate, Weight, Color, Tag,
	Face, Frame, Coface, Coframe, Incidence, Block,
	Construct, Dimension, Vertex, Vector, Pierce, Side,
	Uniform, Global, Query, Texture0, Texture1, Buffers};
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
enum ClickMode {
	AdditiveMode,
	SubtractiveMode,
	RefineMode,
	TransformMode,
	SuspendMode,
	PierceMode,
	RevealMode,
	HideMode,
	TweakMode,
	RandomizeMode,
	ClickModes};
enum MouseMode {
	RotateMode,
	TangentMode,
	TranslateMode,
	MouseModes};
enum RollerMode {
	CylinderMode,
	ClockMode,
	NormalMode,
	ParallelMode,
	ScaleMode,
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
enum Configure {
	AdditiveConf,
	SubtractiveConf,
	RefineConf,
	TransformConf,
	TweakConf,
	RandomizeConf,
	CylinderConf,
	ClockConf,
	NormalConf,
	ParallelConf,
	ScaleConf,
	RotateConf,
	TangetConf,
	TranslateConf,
	SessionConf,
	PolytopeConf,
	FacetConf,
	NumericConf,
	InvariantConf,
	SymbolicConf,
	IncludeConf,
	ExcludeConf,
	PlaneConf,
	MatrixConf,
	GlobalConf,
	SpaceConf,
	RegionConf,
	InflateConf,
	PictureConf,
	SoundConf,
	MetricConf,
	ScriptConf,
	CommandConf,
	ConfigureConf,
	TestConf,
	Configures};
enum ThreadType {
	ReadType,
	WriteType,
	WindowType,
	PolytopeType,
	SystemType,
	ScriptType,
	ThreadTypes};
enum Field {
	AllocField,
	WriteField,
	BindField,
	ReadField,
	Fields};
struct Format
{
	float cursor[2];
	float affine[16];
	float perplane[16];
	float cutoff;
	float slope;
	float aspect;
	float feather;
	float arrow;
	MYuint tagplane;
	char filler1[4-sizeof(MYuint)]; MYuint taggraph;
};
struct Feedback
{
	float pierce[3];
	float normal[3];
	int color;
	int tagbits;
	int plane;
};
struct Data
{
	int file;
	int plane;
	enum ThreadType thread;
	enum Configure conf;
	union {float *matrix; char *text;};
};
struct Update
{
	struct Update *next;
	int file, finish;
	enum Buffer buffer;
	union {int offset; int width;};
	union {int size; int height;};
	MYuint handle;
	union {char *text; float *scalar; MYuint *query;
	struct Format *format; struct Feedback *feedback; struct Data *data;};
	void (*function)(int, struct Update *);
};
struct Render
{
	struct Render *next;
	int file;
	enum Program program;
	enum Space space;
	int base;
	int count;
	int size;
};
struct Response
{
	struct Response *next;
	int file;
	enum ThreadType thread;
};
struct Command
{
	struct Command *next;
	int feedback; int finish;
	struct Update *update[Fields];
	struct Render *render;
	struct Command *redraw;
	struct Command *pierce;
	struct Response *response;
};
struct Invoke
{
	int file;
};
struct Question
{
	int file;
};

#endif
