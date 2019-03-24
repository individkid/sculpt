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
	Point0, // planes from file, only appended to
	Versor, // suppliment to Point0
	Point1, // intersections of Point0, only appended to
	Normal, Coordinate, Color, Weight, Tag, // supplement to Point1
	Point2, // intersections of threat vertices, from Point1 as planes
	Face1, // corners of facets, rewritten when polytope changes
	Triple0, // every triple in Point0, is only appended to
	Triple1, // triples of threat points of boundary to sample
	Feedback, Uniform, Query, Texture0, Texture1, Buffers};
enum Program {
    Draw, // Point1,Normal*3,Coordinate*3,Weight*3,Color*3,Tag*3,Face1 -> display
    Pierce, // Point1,Normal*3,Coordinate*3,Weight*3,Color*3,Tag*3,Face1 -> Pierce,Normal,Color,Plane,Tagbits
    Sect0, // Point0,Versor,Triple0 -> Vertex
    Sect1, // Point1,Triple1 -> Vertex
    Side1, // Point1 -> Scalar
    Side2, // Point2 -> Scalar
    Programs};
enum ClickMode {
	AdditiveMode,
	SubtractiveMode,
	RefineMode,
	TweakMode,
	RandomizeMode,
	PerformMode,
	TransformMode,
	SuspendMode,
	PierceMode,
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
	TweakConf,
	RandomizeConf,
	PerformConf,
	TransformConf,
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
	float basis[9][3];
	float cutoff;
	float slope;
	float aspect;
	float feather[3];
	float arrow[3];
	MYuint enable;
	char filler1[4-sizeof(MYuint)]; MYuint tagplane;
	char filler2[4-sizeof(MYuint)]; MYuint taggraph;
};
struct Feedback
{
	float pierce[3];
	float normal[3];
	float color[3];
	float weight[3];
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
	union {void *data; char *text; float *scalar; MYuint *query;
	struct Format *format; struct Feedback *feedback;};
	void (*function)(struct Update *);
};
struct Render
{
	struct Render *next;
	int file;
	enum Program program;
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
	int tagbits;
	union {
	float *stock;
	struct {
	enum ClickMode click;
	int file;
	int plane;
	float *pierce;};};
};
struct Question
{
	int file;
};

#endif
