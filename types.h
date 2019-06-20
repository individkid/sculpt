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
#define BUFFER_SIZE 100

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
	Feedback, Uniform, Inquery, Texture0, Texture1, Buffers};
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
	SculptConf,
	GlobalConf,
	MatrixConf,
	PlaneConf,
	PictureConf,
	SpaceConf,
	RegionConf,
	InflateConf,
	PolytopeConf,
	IncludeConf,
	ScriptConf,
	PauseConf,
	MacroConf,
	HotkeyConf,
	ConfigureConf,
	Configures};
enum Subconf {
	StartSub,
	StopSub,
	Subconfs};
enum Change {
	GlobalChange,
	MatrixChange,
	PlaneChange,
	RegionChange,
	TextChange,
	Changes};
enum Field {
	AllocField,
	WriteField,
	BindField,
	ReadField,
	Fields};
enum Factor {
	ConstFactor,
	VaryFactor,
	SquareFactor,
	CompFactor,
	Factors};
enum Identity {
	NameIdent,
	NumberIdent,
	Identities};
enum Syncrony {
	StartSync,
	StopSync,
	ScriptSync,
	MetricSync,
	UpdateSync,
	SoundSync,
	Syncronies};
enum Equate {
	ValueEqu, // sample value
	DelayEqu, // wave length
	SchedEqu, // sample rate
	LeftEqu, // left audio
	RightEqu, // right audio
	Equates};
enum Opcode {
	// Thread
	ReadOp, ManipOp, WriteOp, QueryOp, DisplayOp, WindowOp, CommandOp, PointerOp, 
	// Data
	FileOp, PlaneOp, ConfOp,
	SculptOp, ClickOp, MouseOp, RollerOp, TargetOp, TopologyOp, FixedOp,
	BoundariesOp, RegionsOp, PlanesOp, SidesOp,
	SideOp, InsidesOp, OutsidesOp, InsideOp, OutsideOp,
	MatrixOp,
	VersorOp, VectorOp,
	DelayOp, CountOp, IdentOp, ValueOp, MetricOp,
	SubconfOp, SettingOp,
	FilenameOp,
	ScriptOp,
	// Query
	/*FileOp,*/ TextOp,
	// Command
	/*FileOp,*/ FeedbackOp, FinishOp,
	AllocOp, /*WriteOp,*/ BindOp, /*ReadOp,*/
	// Update
	/*FileOp,*/ /*FinishOp,*/
	BufferOp,
	OffsetOp, WidthOp,
	SizeOp, HeightOp,
	DataOp, /*TextOp,*/ ScalarOp, /*QueryOp,*/
	FormatOp, /*FeedbackOp,*/ DatasOp,
	FunctionOp, FunctionsOp,
	// Render
	/*FileOp,*/
	ProgramOp, BaseOp, /*CountOp,*/ /*SizeOp,*/
	// Feedback
	PierceOp, NormalOp, TagbitsOp, /*PlaneOp,*/
	// Format
	CursorOp, AffineOp, PerplaneOp, BasisOp,
	CutoffOp, SlopeOp, AspectOp,
	FeatherOp, ArrowOp, EnableOp,
	// Manip
	/*FileOp,*/ /*PlaneOp,*/
	/*ClickOp,*/ /*TopologyOp,*/ /*FixedOp,*/
	/*VectorOp,*/ /*MatrixOp,*/
	// Sound
	/*FileOp,*/ /*IdentOp,*/ /*ValueOp,*/
	// Sum
	/*CountOp,*/
	// Term
	CoefOp, FactorOp, SquareOp, CompOp,
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
struct Command // (Polytope,Read) -> Window
{
	struct Command *next;
	int file; int feedback; int finish;
	struct Update *update[Fields];
	struct Render *render;
	struct Command *redraw;
	struct Command *pierce;
};
struct Manip // (Window,Read) -> Polytope
{
	struct Manip *next;
	int file; int plane;
	enum ClickMode click;
	enum TopologyMode topology;
	enum FixedMode fixed;
	union {float *vector; float *matrix;};
};
struct State // (Polytope,Window,Script)->Write
{
	struct State *next;
	int file;
	int plane;
	enum Change change;
	union {
	// Polytope->RegionChange->Write
	struct {int side; int insides; int outsides; int *inside; int *outside;};
	// Window->(MatrixChange,GlobalChange)->Write
	float *matrix;
	// Polytope->PlaneChange->Write
	struct {int versor; float *vector;};
	// Script->TextChange->Write
	char *text;};
};
struct Data // Read -> (Polytope,Window,Script,System)
{
	struct Data *next;
	int file; int plane;
	enum Configure conf;
	// Read->InflateConf->Polytope
	union {
	// Read->SculptConf->Window
	struct {enum Sculpt sculpt; union {
	enum ClickMode click; enum MouseMode mouse; enum RollerMode roller;
	enum TargetMode target; enum TopologyMode topology; enum FixedMode fixed;};};
	// Read->SpaceConf->Polytope
	struct {int boundaries; int regions; int *planes; int *sides;};
	// Read->RegionConf->Polytope
	struct {int side; int insides; int outsides; int *inside; int *outside;};
	// Read->(MatrixConf,GlobalConf)->Window
	float *matrix;
	// Read->PlaneConf->Polytope
	struct {int versor; float *vector;};
	// Read->ConfigureConf->Window
	// Read->TimewheelConf->System
	struct {enum Subconf subconf; float setting;};
	// Read->(PictureConf,IncludeConf)->Polytope
	char *filename;
	// Read->(ScriptConf,PauseConf)->Script
	// Read->(MacroConf,HotkeyConf)->Window->Script
	char *script;};
};

struct Term
{
	double coef; enum Factor factor;
	union {int *id; double **ptr;};
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
struct Smart {
	int count;
	char *ptr;
};
struct Sound
{
	struct Sound *next;
	int file;
	int ident;
	int done;
	// Read->(StartSync,StopSync)->System
	enum Syncrony sync;
	double value; // tone envelope phrase helper metric
	union {
	// Read->SoundSync->System
	struct Equ equ[Equates];
	// Read->MetricSync->System
	struct {struct Equ sched; // sample rate
	int id; double *ptr; // result pointer for script
	int count; // number of parameters for script
	int *ids; double **ptrs;
	char *script; // init smart when setting done
 	struct Smart *smart;}; // smart copied to ScriptSync
	// System->ScriptSync->Script
	struct {double *result; // system copies value to *result
	int params; double *values;
 	struct Smart *metric;}; // script to execute per sample rate
 	// UpdateSync on timewheel
	double *update;};
};

struct Query // (Read,Script) -> Polytope
{
	// Script->Query->Polytope
	// Read->Query->Polytope->Script
	struct Query *next;
	int file;
	char *text;
};

#endif
