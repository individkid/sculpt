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
#define BEAT_PERIOD (1.0/44100.0)
#define WAVE_SIZE 1024
#define SAMPLE_RATE 44100
#ifndef DEBUG
#define DEBUG 0
#endif
#define MESSAGE_DEBUG 1
#define HASKELL_DEBUG 2
#define LUA_DEBUG 4
#define CALLBACK_DEBUG 8
#define WINDOW_DEBUG 16

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
enum Sculpt {
	ClickUlpt,
	MouseUlpt,
	RollerUlpt,
	TargetUlpt,
	TopologyUlpt,
	FixedUlpt,
	Sculpts};
enum Configure {
	SculptConf, // Read->Window to change configuration
	PlaneConf, // Read->Polytope Polytope->Write to add plane
	GlobalConf, // Read->Window Window->Write to transform display
	MatrixConf, // Read->Window Window->Write to transform polytope
	ManipConf, // Window->Polytope to change or add plane or topology 
	SpaceConf, // Read->Polytope to change topology
	RegionConf, // Read->Polytope Polytope->Write to change topology
	InflateConf, // Read->Polytope to change topology
	PictureConf, // Read->Polytope to change decoration
	MetricConf, // Read->System System->Script to setup volatile
	MacroConf, // Read->Window Window->Script to setup performance
	HotkeyConf, // Read->Window Window->Script to sentup performance
	ScriptConf, // Read->Script to setup automation
	InvokeConf, // Read->Script to setup automation
	ConfigureConf, // Read->Window to configure constants
	TimewheelConf, // Read->System to start sound
	TestConf, // Read->Polytope to check math
	Configures};
enum Subconf {
	StartSub,
	StopSub,
	Subconfs};
enum Field {
	AllocField,
	WriteField,
	BindField,
	ReadField,
	Fields};
enum Opcode {
	ReadOp, 
	WriteOp, 
	PointerOp, 
	WindowOp,
	FileOp,
	PlaneOp,
	ConfOp,
	SculptOp,
	ClickOp,
	MouseOp,
	RollerOp,
	TargetOp,
	TopoOp,
	FixOp,
	BoundariesOp,
	RegionsOp,
	PlanesOp,
	SidesOp,
	SideOp,
	InsidesOp,
	OutsidesOp,
	InsideOp,
	OutsideOp,
	ModeOp,
	TopologyOp,
	FixedOp,
	PierceOp,
	MatrixOp,
	VersorOp,
	VectorOp,
	DelayOp,
	CountOp,
	IdentOp,
	ValueOp,
	ScriptOp,
	SubconfOp,
	SettingOp,
	TextOp,
	CommandOp, // TODO
	QueryOp, // TODO
	Opcodes};
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
	struct Format *format; struct Feedback *feedback; int datas;};
	union {void (*function)(struct Update *); int functions;};
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
struct Command
{
	// Read->Command->Window
	struct Command *next;
	int file; int feedback; int finish;
	struct Update *update[Fields];
	struct Render *render;
	struct Command *redraw;
	struct Command *pierce;
};
struct Data
{
	struct Data *next;
	int file;
	int plane;
	enum Configure conf;
	// Read->InflateConf->Polytope->Command->Window
	union {
	// Read->SculptConf->Window
	struct {enum Sculpt sculpt; union {
	enum ClickMode click; enum MouseMode mouse; enum RollerMode roller;
	enum TargetMode target; enum TopologyMode topo; enum FixedMode fix;};};
	// Read->SpaceConf->Polytope->Command->Window
	struct {int boundaries; int regions; int *planes; int *sides;};
	// Read->RegionConf->Polytope->Command->Window
	struct {int side; int insides; int outsides; int *inside; int *outside;};
	// Read->(MatrixConf,GlobalConf)->Window->Write
	float *matrix;
	// Window->ManipConf->Polytope->(PlaneConf,RegionConf)->Write
	struct {enum ClickMode mode; enum TopologyMode topology; enum FixedMode fixed;
	union {float *pierce; float *manip;};};
	// Read->PlaneConf->Polytope->Command->Window
	struct {int versor; float *vector;};
	// Read->MetricConf->System->Script
	struct {float delay; int count; int *ident; double *value; char *script;};
	// Read->ConfigureConf->Window
	// Read->TimewheelConf->System
	struct {enum Subconf subconf; float setting;};
	// Read->PictureConf->Polytope->Command->Window
	// Read->TestConf->Polytope
	// Read->(ScriptConf,InvokeConf)->Script
	// Read->(MacroConf,HotkeyConf)->Window->Script
	char *text;};
};
struct Term
{
	double coef;
	union {int id; double *ptr;} factor;
	union {int id; double *ptr;} square;
	union {int id; double *ptr;} comp;
};
struct Sum
{
	int count;
	struct Term *term;
};
struct Equ
{
	struct Sum numer;
	struct Sum denom;
};
struct Sound
{
	// Read->Sound->System
	struct Sound *next;
	int file;
	int ident;
	double value; // tone envelope phrase or helper
	struct Equ equat; // new value after delay
	struct Equ delay; // wavelength
	struct Equ sched; // sample rate
	struct Equ left; // directly audible left
	struct Equ right; // directly audible right
};

struct None
{
	struct None *next;
};

struct Query
{
	// Script->Query->Polytope
	struct Query *next;
};

union Symbol
{
	int count;
	struct Data *data;
	struct Command *command;
	struct Update *update;
	struct Render *render;
	struct Format *format;
	struct Feedback *feedback;
	struct Sound *sound;
	struct Term *term;
	double *dptr;
	float *fptr;
	int *iptr;
	char *cptr;
};

#endif
