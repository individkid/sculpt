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
#define BREAK 97
#ifndef DEBUG
#define DEBUG 0
#endif

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
	MacroConf,
	ScriptConf,
	InvokeConf,
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
// Read->Window for bringup  
// Script->Window for queries to microcode  
// Polytope->Window for changing what is displayed  
struct Command
{
	struct Command *next;
	int feedback; int finish; int file;
	struct Update *update[Fields];
	struct Render *render;
	struct Command *redraw;
	struct Command *pierce;
};
struct Data
{
	int file;
	int plane;
	enum Configure conf; // Script->*->Write for side effects  
	// Read->(AdditiveConf..AbsoluteConf)->Window for changing modes  
	// Read->InflateConf->Polytope for changing which regions are in the polytope  
	// Window->AdditiveConf,SubtractiveConf->Polytope for sculpting polytope  
	union {
	// Read->SpaceConf->Polytope for creating sample of space  
	// Script->SpaceConf->Polytope for feedback from topology  
	struct {int boundaries; int regions; int *planes; int **sides;};
	// Read->RegionConf->Polytope for changing which regions are in the polytope  
	// Script->RegionConf->Polytope for feedback from topology  
	// Polytope->RegionConf->Write for changing whether region is in polytope  
	struct {int insides; int outsides; int *inside; int *outside; int side;};
	// Read->PlaneConf->Polytope for adding or changing planes  
	// Script->PlaneConf->Polytope for feedback from topology  
	// Polytope->PlaneConf->Write for appending manipulated or randomized planes  
	struct {int versor; float *vector;};
	// Read->(ScriptConf,InvokeConf)->Script for setting up scripts  
	// Read->MacroConf->Polytope for attaching scripts to planes  
	// Read->MetricConf->System for allowing system to query state  
	// Window->MacroConf->Polytope for tagbits associated with click  
	// Polytope->MacroConf->Script for starting macro from click  
	// System->MetricConf->Script for getting value from metric  
	struct {int tagbits; union {float *argument; int *ident;}; char *script;};
	// Window->TweakConf->Polytope for tweaking planes  
	// Window->RefineConf->Polytope for adding planes  
	struct {enum TopologyMode topology; enum FixedMode fixed; float *pierce;};
	// Read->(MatrixConf,GlobalConf)->Window for applying transformations from other processes  
	// Window->(MatrixConf,GlobalConf)->Write for recording transformations  
	// Window->TransformConf->Polytope for manipulating planes  
	struct {int seqnum; float *matrix;};
	// Read->SoundConf->System for changing sound  
	// Script->SoundConf->System for getting stock values  
	// TODO
	// Read->ConfigureConf->Window for changing configurations  
	// TODO
	// Read->(PictureConf,MacroConf)->Polytope for decorating planes  
	// Read->TestConf->Polytope for testing topology functions
	char *text;};
};

#endif
