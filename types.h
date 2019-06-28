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

// Command
enum Source {
	ConfigureSource,
	ModeSource,
	MatrixSource,
	GlobalSource,
	PolytopeSource,
	Sources};
enum Subconf {
	StartSub,
	StopSub,
	Subconfs};
enum Sculpt {
	ClickUlpt,
	MouseUlpt,
	RollerUlpt,
	TargetUlpt,
	TopologyUlpt,
	FixedUlpt,
	Sculpts};
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
enum Field {
	AllocField,
	WriteField,
	BindField,
	ReadField,
	Fields};
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
	struct Command *next; int file;
	enum Source source;
	union {
	// Read->ConfigureSource->Window
	struct {enum Subconf subconf; float setting;};
	// Read->ModeSource->Window
	struct {enum Sculpt sculpt; union {
	enum ClickMode click;
	enum MouseMode mouse;
	enum RollerMode roller;
	enum TargetMode target;
	enum TopologyMode topology;
	enum FixedMode fixed;};};
	// Read->(MatrixSource,GlobalSource)->Window
	float *matrix;
	// Command->PolytopeSource->Window
 	struct {int feedback; int finish;
 	struct Update *update[Fields];
	struct Render *render;
	struct Command *redraw;
	struct Command *pierce;};};
};

// Data
enum Configure {
	InflateConf,
	SpaceConf,
	RegionConf,
	PlaneConf,
	PictureConf,
	OnceConf,
	ChangeConf,
	MacroConf,
	HotkeyConf,
	RelativeConf,
	AbsoluteConf,
	RefineConf,
	ManipConf,
	PressConf,
	ClickConf,	
	AdditiveConf,
	SubtractiveConf,
	Configures};
enum Function {
	Functions};
struct Data // (Read,Window) -> Polytope
{
	struct Data *next; int file; int plane;
	enum Configure conf;
	// Read->InflateConf->Polytope
	union {
	// Read->SpaceConf->Polytope
	struct {int boundaries; int regions; int *planes; int *sides;};
	// Read->RegionConf->Polytope
	struct {int side; int insides; int outsides; int *inside; int *outside;};
	// Read->PlaneConf->Polytope
	struct {int versor; float *vector;};
	// Read->PictureConf->Polytope
	char *filename;
	// Read->(OnceConf,ChangeConf)->Polytope
	struct {enum Function func; int count; int *specify; char *script;};
	// Read->HotkeyConf->Polytope
	struct {char key; char *hotkey;};
	// Read->MacroConf->Polytope
	char *macro;
	// Window->RelativeConf->Polytope
	struct {float *fixed; enum TopologyMode relative;};
	// Window->AbsoluteConf->Polytope
	enum TopologyMode absolute;
	// Window->RefineConf->Polytope
	float *pierce;
	// Window->ManipConf->Poltope
	float *matrix;
	// Window->PressConf->Polytope
	char press;};
	// Window->ClickConf->Polytope
	// Window->AdditiveConf->Polytope
	// Window->SubtractiveConf->Polytope
};

// Sound
enum Event {
	StartEvent,
	StopEvent,
	SoundEvent,
	ScriptEvent,
	UpdateEvent,
	Events};
enum Equate {
	ValueEqu, // sample value
	DelayEqu, // wave length
	SchedEqu, // sample rate
	LeftEqu, // left audio
	RightEqu, // right audio
	Equates};
enum Factor {
	ConstFactor,
	VaryFactor,
	SquareFactor,
	CompFactor,
	Factors};
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
struct Sound
{
	struct Sound *next; int file; int ident;
	int done; // initialize ptrs if not done
	double value; // tone envelope phrase helper metric
	enum Event event;
	// Read->(StartEvent,StopEvent)->System
	union {
	// Read->SoundEvent->System
	struct Equ equ[Equates];
	// Read->ScriptEvent->System
	struct {struct Equ sched; // sample rate
	int count; // number of parameters for script
	int *ids; double **ptrs; // initialize to set done
	char *script;}; // init Smart for Query
 	// UpdateEvent on timewheel
	double *update;}; // assign when UpdateSync allocated
};

// State
enum Change {
	SculptChange,
	GlobalChange,
	MatrixChange,
	PlaneChange,
	RegionChange,
	TextChange,
	Changes};
struct State // (Polytope,Window,Script)->Write
{
	struct State *next; int file; int plane;
	enum Change change;
	union {
	// Window->SculptChange->Write
	struct {enum Sculpt sculpt; union {
	enum ClickMode click;
	enum MouseMode mouse;
	enum RollerMode roller;
	enum TargetMode target;
	enum TopologyMode topology;
	enum FixedMode fixed;};};
	// Polytope->RegionChange->Write
	struct {int side; int insides; int outsides; int *inside; int *outside;};
	// Window->(MatrixChange,GlobalChange)->Write
	float *matrix;
	// Polytope->PlaneChange->Write
	struct {int versor; float *vector;};
	// Script->TextChange->Write
	char *text;};
};

// Query
struct Smart {
	int count;
	char *ptr;
};
struct Query
{
	struct Query *next; int file;
 	struct Smart smart;
	union {
	// no response to Read
	double value; // for response to System
	struct {int length; char *stream;};}; // for response to Polytope
};

enum Opcode {
	// Sideband
	// TODO for new Query pointer
	// Message
	ReadOp, WriteOp, ScriptOp, WindowOp, CommandOp,
	// Command
	FileOp,
	SourceOp,
	SubconfOp, SettingOp,
	SculptOp, ClickOp, MouseOp, RollerOp, TargetOp, TopologyOp, FixedOp,
	MatrixOp,
	FeedbackOp, FinishOp,
	AllocOp, /*WriteOp,*/ BindOp, /*ReadOp,*/
	// Update
	/*FileOp,*/ /*FinishOp,*/
	BufferOp,
	OffsetOp, WidthOp,
	SizeOp, HeightOp,
	DataOp, TextOp, ScalarOp, QueryOp,
	FormatOp, /*FeedbackOp,*/ DatasOp,
	FunctionOp, FunctionsOp,
	// Render
	/*FileOp,*/
	ProgramOp, BaseOp, CountOp, /*SizeOp,*/
	// Feedback
	PierceOp, NormalOp, TagbitsOp, PlaneOp,
	// Format
	CursorOp, AffineOp, PerplaneOp, BasisOp,
	CutoffOp, SlopeOp, AspectOp,
	FeatherOp, ArrowOp, EnableOp,
	// Data
	/*FileOp, PlaneOp,*/ ConfOp,
	BoundariesOp, RegionsOp, PlanesOp, SidesOp,
	SideOp, InsidesOp, OutsidesOp, InsideOp, OutsideOp,
	VersorOp, VectorOp,
	FilenameOp,
	FuncOp, /*CountOp,*/ SpecifyOp, /*ScriptOp,*/
	KeyOp, HotkeyOp,
	MacroOp,
	/*FixedOp,*/ RelativeOp,
	AbsoluteOp,
	/*PierceOp,*/
	/*MatrixOp,*/
	PressOp,
	// Sound
	/*FileOp,*/ IdentOp, DoneOp, ValueOp, EventOp,
	/*ValueOp,*/ DelayOp, SchedOp, LeftOp, RightOp,
	// Equ
	NumerOp, DenomOp,
	// Sum
	/*CountOp,*/
	// Term
	CoefOp, FactorOp,
	ConstOp, VaryOp, SquareOp, CompOp,
	// State
	/*FileOp, PlaneOp,*/
	ChangeOp,
	/*SculptOp, ClickOp, MouseOp, RollerOp, TargetOp, TopologyOp, FixedOp,*/
	/*SideOp, InsidesOp, OutsidesOp, InsideOp, OutsideOp,*/
	/*MatrixOp,*/
	/*VersorOp, VectorOp,*/
	/*TextOp,*/
	// Query
	/*FileOp,*/ SmartOp,
	/*ValueOp,*/
	LengthOp, StreamOp,
	Opcodes};

#endif
