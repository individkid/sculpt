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
#define INVALID 1.0e37
#define MMAP 256

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
enum TopologyMode {
	NumericMode,
	InvariantMode,
	SymbolicMode,
	TopologyModes};
enum FixedMode {
	RelativeMode,
	AbsoluteMode,
	FixedModes};
enum Configure {
	AdditiveConf,
	SubtractiveConf,
	RefineConf,
	TweakConf,
	PerformConf,
	TransformConf,
	CylinderConf,
	ClockConf,
	NormalConf,
	ParallelConf,
	ScaleConf,
	RotateConf,
	TangentConf,
	TranslateConf,
	SessionConf,
	PolytopeConf,
	FacetConf,
	NumericConf,
	InvariantConf,
	SymbolicConf,
	RelativeConf,
	AbsoluteConf,
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
enum Field {
	AllocField,
	WriteField,
	BindField,
	ReadField,
	Fields};
enum ModeType {
	ClickType,
	MouseType,
	RollerType,
	TargetType,
	TopologyType,
	FixedType,
	ModeTypes};
struct Data
{
	int file;
	int plane;
	enum Configure conf;
	union {
	struct {int boundaries; int regions; int *planes; int **sides;};
	struct {int insides; int outsides; int *inside; int *outside; int side;};
	struct {int versor; float *vector;};
	struct {int number; enum TargetMode target; float *matrix;};
	char *text;};
};
struct Sync
{
	int file;
	int plane;
	int number;
	enum TargetMode target;
	float *matrix;
};
struct Mode
{
	int file;
	int plane;
	enum ModeType mode;
	union {
	enum ClickMode click;
	enum MouseMode mouse;
	enum RollerMode roller;
	enum TargetMode target;
	enum TopologyMode topology;
	enum FixedMode fixed;};
};
struct Action
{
	int file;
	int plane;
	enum ClickMode click; // additive subtractive refine tweak or transform facet
	enum TopologyMode topology; // tweak
	enum FixedMode fixed; // tweak
	union {float *pierce; // refine or tweak relative
	float *matrix;}; // transform facet
};
struct Invoke
{
	int file;
	int plane;
	int tagbits;
	float *vector;
};
struct Question
{
	int file;
};
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
	int tagbits;
	int plane;
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

#endif
