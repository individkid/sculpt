/*
*    haskell.c ffi for transfering c structs between pipes and haskell
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

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/errno.h>
#include "haskell.h"

static int debug = 0;

void fatal(const char *str, int wrt, const char *file, int line)
{
    fprintf(stderr,"error: %s; wrt %d; file %s; line %d\n",str,wrt,file,line);
    exit(-1);
}

void error(const char *str, int wrt, const char *file, int line)
{
	if (debug) {if (DEBUG & HASKELL_DEBUG) printf("haskell done\n"); exit(0);}
	fatal(str,wrt,file,line);
}

void setDebug(int val)
{
	debug = val;
}

const char *enamerate(enum Opcode opcode)
{
	switch (opcode) {
	// Sideband
	// TODO for new Query pointer
	// Message
	case (ReadOp): return "ReadOp";
	case (WriteOp): return "WriteOp";
	case (ScriptOp): return "ScriptOp";
	case (WindowOp): return "WindowOp";
	case (CommandOp): return "CommandOp";
	// Command
	case (FileOp): return "FileOp";
	case (SourceOp): return "SourceOp";
	case (SubconfOp): return "SubconfOp";
	case (SettingOp): return "SettingOp";
	case (SculptOp): return "SculptOp";
	case (ClickOp): return "ClickOp";
	case (MouseOp): return "MouseOp";
	case (RollerOp): return "RollerOp";
	case (TargetOp): return "TargetOp";
	case (TopologyOp): return "TopologyOp";
	case (FixedOp): return "FixedOp";
	case (MatrixOp): return "MatrixOp";
	case (FeedbackOp): return "FeedbackOp";
	case (FinishOp): return "FinishOp";
	case (AllocOp): return "AllocOp";
	/*case (WriteOp): return "WriteOp";*/
	case (BindOp): return "BindOp";
	/*case (ReadOp): return "ReadOp";*/
	case (PlaneOp): return "PlaneOp";
	case (KeyOp): return "KeyOp";
	/*case (ScriptOp): return "ScriptOp";*/
	// Update
	/*case (FileOp): return "FileOp";*/
	/*case (FinishOp): return "FinishOp";*/
	case (BufferOp): return "BufferOp";
	case (OffsetOp): return "OffsetOp";
	case (WidthOp): return "WidthOp";
	case (SizeOp): return "SizeOp";
	case (HeightOp): return "HeightOp";
	case (DataOp): return "DataOp";
	case (TextOp): return "TextOp";
	case (ScalarOp): return "ScalarOp";
	case (QueryOp): return "QueryOp";
	case (FormatOp): return "FormatOp";
	/*case (FeedbackOp): return "FeedbackOp";*/
	case (DatasOp): return "DatasOp";
	case (FunctionOp): return "FunctionOp";
	case (FunctionsOp): return "FunctionsOp";
	// Render
	/*case (FileOp): return "FileOp";*/
	case (ProgramOp): return "ProgramOp";
	case (BaseOp): return "BaseOp";
	case (CountOp): return "CountOp";
	/*case (SizeOp): return "SizeOp";*/
	// Feedback
	case (PierceOp): return "PierceOp";
	case (NormalOp): return "NormalOp";
	case (TagbitsOp): return "TagbitsOp";
	/*case (PlaneOp): return "PlaneOp";*/
	// Format
	case (CursorOp): return "CursorOp";
	case (AffineOp): return "AffineOp";
	case (PerplaneOp): return "PerplaneOp";
	case (BasisOp): return "BasisOp";
	case (CutoffOp): return "CutoffOp";
	case (SlopeOp): return "SlopeOp";
	case (AspectOp): return "AspectOp";
	case (FeatherOp): return "FeatherOp";
	case (ArrowOp): return "ArrowOp";
	case (EnableOp): return "EnableOp";
	case (TagplaneOp): return "TagplaneOp";
	case (TaggraphOp): return "TaggraphOp";
	// Data
	/*case (FileOp): return "FileOp";
	case (PlaneOp): return "PlaneOp";*/
	case (ConfOp): return "ConfOp";
	case (BoundariesOp): return "BoundariesOp";
	case (RegionsOp): return "RegionsOp";
	case (PlanesOp): return "PlanesOp";
	case (SidesOp): return "SidesOp";
	case (SideOp): return "SideOp";
	case (InsidesOp): return "InsidesOp";
	case (OutsidesOp): return "OutsidesOp";
	case (InsideOp): return "InsideOp";
	case (OutsideOp): return "OutsideOp";
	case (VersorOp): return "VersorOp";
	case (VectorOp): return "VectorOp";
	case (FilenameOp): return "FilenameOp";
	case (FuncOp): return "FuncOp";
	/*case (CountOp): return "CountOp";*/
	case (SpecifyOp): return "SpecifyOp";
	/*case (ScriptOp): return "ScriptOp";*/
	/*case (FixedOp): return "FixedOp";*/
	case (RelativeOp): return "RelativeOp";
	case (AbsoluteOp): return "AbsoluteOp";
	/*case (MatrixOp): return "MatrixOp";*/
	case (PressOp): return "PressOp";
	// Sound
	case (DoneOp): return "DoneOp";
	case (IdentOp): return "IdentOp";
	case (ValueOp): return "ValueOp";
	case (EventOp): return "EventOp";
	/*case (ValueOp): return "ValueOp";*/
	case (DelayOp): return "DelayOp";
	case (SchedOp): return "SchedOp";
	case (LeftOp): return "LeftOp";
	case (RightOp): return "RightOp";
	/*case (SchedOp): return "SchedOp";*/
	/*case (CountOp): return "CountOp";*/
	case (IdsOp): return "IdsOp";
	case (PtrsOp): return "PtrsOp";
	/*case (ScriptOp): return "ScriptOp";*/
	case (IdOp): return "IdOp";
	case (UpdateOp): return "UpdateOp";
	// Equ
	case (NumerOp): return "NumerOp";
	case (DenomOp): return "DenomOp";
	// Sum
	/*case (CountOp): return "CountOp";*/
	// Term
	case (CoefOp): return "CoefOp";
	case (FactorOp): return "FactorOp";
	/*case (IdOp): return "IdOp";*/
	case (PtrOp): return "PtrOp";
	// State
	/*case (FileOp): return "FileOp";
	case (PlaneOp): return "PlaneOp";*/
	case (ChangeOp): return "ChangeOp";
	/*case (SculptOp): return "SculptOp";
	case (ClickOp): return "ClickOp";
	case (MouseOp): return "MouseOp";
	case (RollerOp): return "RollerOp";
	case (TargetOp): return "TargetOp";
	case (TopologyOp): return "TopologyOp";
	case (FixedOp): return "FixedOp";*/
	/*case (SideOp): return "SideOp";
	case (InsidesOp): return "InsidesOp";
	case (OutsidesOp): return "OutsidesOp";
	case (InsideOp): return "InsideOp";
	case (OutsideOp): return "OutsideOp";*/
	/*case (MatrixOp): return "MatrixOp";*/
	/*case (VersorOp): return "VersorOp";
	case (VectorOp): return "VectorOp";*/
	/*case (TextOp): return "TextOp";*/
	// Query
	/*case (ScriptOp): return "ScriptOp";*/
 	case (GivenOp): return "GivenOp";
	/*case (FileOp): return "FileOp";*/
 	case (LengthOp): return "LengthOp";
	case (DoublesOp): return "DoublesOp";
	case (FloatsOp): return "FloatsOp";
	case (IntsOp): return "IntsOp";
	case (CharsOp): return "CharsOp";
	/*case (KeyOp): return "KeyOp";*/
	/*case (PlaneOp): return "PlaneOp";*/
	case (Opcodes): return "Opcodes";
	default: fatal("unknown opcode",opcode,__FILE__,__LINE__);}
	return 0;
}

int enumerate(char *name)
{
	// Sideband
	// TODO for new Query pointer
	// Message
	if (strcmp(name,"ReadOp") == 0) return ReadOp;
	if (strcmp(name,"WriteOp") == 0) return WriteOp;
	if (strcmp(name,"ScriptOp") == 0) return ScriptOp;
	if (strcmp(name,"WindowOp") == 0) return WindowOp;
	if (strcmp(name,"CommandOp") == 0) return CommandOp;
	// Command
	if (strcmp(name,"FileOp") == 0) return FileOp;
	if (strcmp(name,"SourceOp") == 0) return SourceOp;
	if (strcmp(name,"SubconfOp") == 0) return SubconfOp;
	if (strcmp(name,"SettingOp") == 0) return SettingOp;
	if (strcmp(name,"SculptOp") == 0) return SculptOp;
	if (strcmp(name,"ClickOp") == 0) return ClickOp;
	if (strcmp(name,"MouseOp") == 0) return MouseOp;
	if (strcmp(name,"RollerOp") == 0) return RollerOp;
	if (strcmp(name,"TargetOp") == 0) return TargetOp;
	if (strcmp(name,"TopologyOp") == 0) return TopologyOp;
	if (strcmp(name,"FixedOp") == 0) return FixedOp;
	if (strcmp(name,"MatrixOp") == 0) return MatrixOp;
	if (strcmp(name,"FeedbackOp") == 0) return FeedbackOp;
	if (strcmp(name,"FinishOp") == 0) return FinishOp;
	if (strcmp(name,"AllocOp") == 0) return AllocOp;
	/*if (strcmp(name,"WriteOp") == 0) return WriteOp;*/
	if (strcmp(name,"BindOp") == 0) return BindOp;
	/*if (strcmp(name,"ReadOp") == 0) return ReadOp;*/
	if (strcmp(name,"PlaneOp") == 0) return PlaneOp;
	if (strcmp(name,"KeyOp") == 0) return KeyOp;
	/*if (strcmp(name,"ScriptOp") == 0) return ScriptOp;*/
	// Update
	/*if (strcmp(name,"FileOp") == 0) return FileOp;*/
	/*if (strcmp(name,"FinishOp") == 0) return FinishOp;*/
	if (strcmp(name,"BufferOp") == 0) return BufferOp;
	if (strcmp(name,"OffsetOp") == 0) return OffsetOp;
	if (strcmp(name,"WidthOp") == 0) return WidthOp;
	if (strcmp(name,"SizeOp") == 0) return SizeOp;
	if (strcmp(name,"HeightOp") == 0) return HeightOp;
	if (strcmp(name,"DataOp") == 0) return DataOp;
	if (strcmp(name,"TextOp") == 0) return TextOp;
	if (strcmp(name,"ScalarOp") == 0) return ScalarOp;
	if (strcmp(name,"QueryOp") == 0) return QueryOp;
	if (strcmp(name,"FormatOp") == 0) return FormatOp;
	/*if (strcmp(name,"FeedbackOp") == 0) return FeedbackOp;*/
	if (strcmp(name,"DatasOp") == 0) return DatasOp;
	if (strcmp(name,"FunctionOp") == 0) return FunctionOp;
	if (strcmp(name,"FunctionsOp") == 0) return FunctionsOp;
	// Render
	/*if (strcmp(name,"FileOp") == 0) return FileOp;*/
	if (strcmp(name,"ProgramOp") == 0) return ProgramOp;
	if (strcmp(name,"BaseOp") == 0) return BaseOp;
	if (strcmp(name,"CountOp") == 0) return CountOp;
	/*if (strcmp(name,"SizeOp") == 0) return SizeOp;*/
	// Feedback
	if (strcmp(name,"PierceOp") == 0) return PierceOp;
	if (strcmp(name,"NormalOp") == 0) return NormalOp;
	if (strcmp(name,"TagbitsOp") == 0) return TagbitsOp;
	/*if (strcmp(name,"PlaneOp") == 0) return PlaneOp;*/
	// Format
	if (strcmp(name,"CursorOp") == 0) return CursorOp;
	if (strcmp(name,"AffineOp") == 0) return AffineOp;
	if (strcmp(name,"PerplaneOp") == 0) return PerplaneOp;
	if (strcmp(name,"BasisOp") == 0) return BasisOp;
	if (strcmp(name,"CutoffOp") == 0) return CutoffOp;
	if (strcmp(name,"SlopeOp") == 0) return SlopeOp;
	if (strcmp(name,"AspectOp") == 0) return AspectOp;
	if (strcmp(name,"FeatherOp") == 0) return FeatherOp;
	if (strcmp(name,"ArrowOp") == 0) return ArrowOp;
	if (strcmp(name,"EnableOp") == 0) return EnableOp;
	// Data
	/*if (strcmp(name,"FileOp") == 0) return FileOp;
	if (strcmp(name,"PlaneOp") == 0) return PlaneOp;*/
	if (strcmp(name,"ConfOp") == 0) return ConfOp;
	if (strcmp(name,"BoundariesOp") == 0) return BoundariesOp;
	if (strcmp(name,"RegionsOp") == 0) return RegionsOp;
	if (strcmp(name,"PlanesOp") == 0) return PlanesOp;
	if (strcmp(name,"SidesOp") == 0) return SidesOp;
	if (strcmp(name,"SideOp") == 0) return SideOp;
	if (strcmp(name,"InsidesOp") == 0) return InsidesOp;
	if (strcmp(name,"OutsidesOp") == 0) return OutsidesOp;
	if (strcmp(name,"InsideOp") == 0) return InsideOp;
	if (strcmp(name,"OutsideOp") == 0) return OutsideOp;
	if (strcmp(name,"VersorOp") == 0) return VersorOp;
	if (strcmp(name,"VectorOp") == 0) return VectorOp;
	if (strcmp(name,"FilenameOp") == 0) return FilenameOp;
	if (strcmp(name,"FuncOp") == 0) return FuncOp;
	/*if (strcmp(name,"CountOp") == 0) return CountOp;*/
	if (strcmp(name,"SpecifyOp") == 0) return SpecifyOp;
	/*if (strcmp(name,"ScriptOp") == 0) return ScriptOp;*/
	/*if (strcmp(name,"FixedOp") == 0) return FixedOp;*/
	if (strcmp(name,"RelativeOp") == 0) return RelativeOp;
	if (strcmp(name,"AbsoluteOp") == 0) return AbsoluteOp;
	/*if (strcmp(name,"PierceOp") == 0) return PierceOp;*/
	/*if (strcmp(name,"MatrixOp") == 0) return MatrixOp;*/
	if (strcmp(name,"PressOp") == 0) return PressOp;
	// Sound
	if (strcmp(name,"DoneOp") == 0) return DoneOp;
	if (strcmp(name,"IdentOp") == 0) return IdentOp;
	if (strcmp(name,"ValueOp") == 0) return ValueOp;
	if (strcmp(name,"EventOp") == 0) return EventOp;
	/*if (strcmp(name,"ValueOp") == 0) return ValueOp;*/
	if (strcmp(name,"DelayOp") == 0) return DelayOp;
	if (strcmp(name,"SchedOp") == 0) return SchedOp;
	if (strcmp(name,"LeftOp") == 0) return LeftOp;
	if (strcmp(name,"RightOp") == 0) return RightOp;
	/*if (strcmp(name,"SchedOp") == 0) return SchedOp;*/
	if (strcmp(name,"CountOp") == 0) return CountOp;
	if (strcmp(name,"IdsOp") == 0) return IdsOp;
	if (strcmp(name,"PtrsOp") == 0) return PtrsOp;
	/*if (strcmp(name,"ScriptOp") == 0) return ScriptOp;*/
	if (strcmp(name,"IdOp") == 0) return IdOp;
	if (strcmp(name,"UpdateOp") == 0) return UpdateOp;
	// Equ
	if (strcmp(name,"NumerOp") == 0) return NumerOp;
	if (strcmp(name,"DenomOp") == 0) return DenomOp;
	// Sum
	/*if (strcmp(name,"CountOp") == 0) return CountOp;*/
	// Term
	if (strcmp(name,"CoefOp") == 0) return CoefOp;
	if (strcmp(name,"FactorOp") == 0) return FactorOp;
	if (strcmp(name,"IdOp") == 0) return IdOp;
	if (strcmp(name,"PtrOp") == 0) return PtrOp;
	// State
	/*if (strcmp(name,"FileOp") == 0) return FileOp;
	if (strcmp(name,"PlaneOp") == 0) return PlaneOp;*/
	if (strcmp(name,"ChangeOp") == 0) return ChangeOp;
	/*if (strcmp(name,"SculptOp") == 0) return SculptOp;
	if (strcmp(name,"ClickOp") == 0) return ClickOp;
	if (strcmp(name,"MouseOp") == 0) return MouseOp;
	if (strcmp(name,"RollerOp") == 0) return RollerOp;
	if (strcmp(name,"TargetOp") == 0) return TargetOp;
	if (strcmp(name,"TopologyOp") == 0) return TopologyOp;
	if (strcmp(name,"FixedOp") == 0) return FixedOp;*/
	/*if (strcmp(name,"SideOp") == 0) return SideOp;
	if (strcmp(name,"InsidesOp") == 0) return InsidesOp;
	if (strcmp(name,"OutsidesOp") == 0) return OutsidesOp;
	if (strcmp(name,"InsideOp") == 0) return InsideOp;
	if (strcmp(name,"OutsideOp") == 0) return OutsideOp;*/
	/*if (strcmp(name,"MatrixOp") == 0) return MatrixOp;*/
	/*if (strcmp(name,"VersorOp") == 0) return VersorOp;
	if (strcmp(name,"VectorOp") == 0) return VectorOp;*/
	/*if (strcmp(name,"TextOp") == 0) return TextOp;*/
	// Query
	/*if (strcmp(name,"ScriptOp") == 0) return ScriptOp;*/
	if (strcmp(name,"GivenOp") == 0) return GivenOp;
	/*if (strcmp(name,"FileOp") == 0) return FileOp;*/
	if (strcmp(name,"LengthOp") == 0) return LengthOp;
	if (strcmp(name,"DoublesOp") == 0) return DoublesOp;
	if (strcmp(name,"FloatsOp") == 0) return FloatsOp;
	if (strcmp(name,"IntsOp") == 0) return IntsOp;
	if (strcmp(name,"CharsOp") == 0) return CharsOp;
	/*if (strcmp(name,"KeyOp") == 0) return KeyOp;*/
	/*if (strcmp(name,"PlaneOp") == 0) return PlaneOp;*/
	if (strcmp(name,"Opcodes") == 0) return Opcodes;
	// Source
	if (strcmp(name,"ConfigureSrc") == 0) return ConfigureSrc;
	if (strcmp(name,"ModeSrc") == 0) return ModeSrc;
	if (strcmp(name,"MatrixSrc") == 0) return MatrixSrc;
	if (strcmp(name,"GlobalSrc") == 0) return GlobalSrc;
	if (strcmp(name,"PolytopeSrc") == 0) return PolytopeSrc;
	if (strcmp(name,"MacroSrc") == 0) return MacroSrc;
	if (strcmp(name,"HotkeySrc") == 0) return HotkeySrc;
	if (strcmp(name,"Sources") == 0) return Sources;
	// Subconf
	if (strcmp(name,"StartSub") == 0) return StartSub;
	if (strcmp(name,"StopSub") == 0) return StopSub;
	if (strcmp(name,"Subconfs") == 0) return Subconfs;
	// Sculpt
	if (strcmp(name,"ClickUlpt") == 0) return ClickUlpt;
	if (strcmp(name,"MouseUlpt") == 0) return MouseUlpt;
	if (strcmp(name,"RollerUlpt") == 0) return RollerUlpt;
	if (strcmp(name,"TargetUlpt") == 0) return TargetUlpt;
	if (strcmp(name,"TopologyUlpt") == 0) return TopologyUlpt;
	if (strcmp(name,"FixedUlpt") == 0) return FixedUlpt;
	if (strcmp(name,"Sculpts") == 0) return Sculpts;
	// ClickMode
	if (strcmp(name,"AdditiveMode") == 0) return AdditiveMode;
	if (strcmp(name,"SubtractiveMode") == 0) return SubtractiveMode;
	if (strcmp(name,"RefineMode") == 0) return RefineMode;
	if (strcmp(name,"TweakMode") == 0) return TweakMode;
	if (strcmp(name,"PerformMode") == 0) return PerformMode;
	if (strcmp(name,"TransformMode") == 0) return TransformMode;
	if (strcmp(name,"SuspendMode") == 0) return SuspendMode;
	if (strcmp(name,"PierceMode") == 0) return PierceMode;
	if (strcmp(name,"ClickModes") == 0) return ClickModes;
	// MouseMode
	if (strcmp(name,"RotateMode") == 0) return RotateMode;
	if (strcmp(name,"TangentMode") == 0) return TangentMode;
	if (strcmp(name,"TranslateMode") == 0) return TranslateMode;
	if (strcmp(name,"MouseModes") == 0) return MouseModes;
	// RollerMode
	if (strcmp(name,"CylinderMode") == 0) return CylinderMode;
	if (strcmp(name,"ClockMode") == 0) return ClockMode;
	if (strcmp(name,"NormalMode") == 0) return NormalMode;
	if (strcmp(name,"ParallelMode") == 0) return ParallelMode;
	if (strcmp(name,"ScaleMode") == 0) return ScaleMode;
	if (strcmp(name,"RollerModes") == 0) return RollerModes;
	// TargetMode
	if (strcmp(name,"SessionMode") == 0) return SessionMode;
	if (strcmp(name,"PolytopeMode") == 0) return PolytopeMode;
	if (strcmp(name,"FacetMode") == 0) return FacetMode;
	if (strcmp(name,"TargetModes") == 0) return TargetModes;
	// TopologyMode
	if (strcmp(name,"NumericMode") == 0) return NumericMode;
	if (strcmp(name,"InvariantMode") == 0) return InvariantMode;
	if (strcmp(name,"SymbolicMode") == 0) return SymbolicMode;
	if (strcmp(name,"TopologyModes") == 0) return TopologyModes;
	// FixedMode
	if (strcmp(name,"RelativeMode") == 0) return RelativeMode;
	if (strcmp(name,"AbsoluteMode") == 0) return AbsoluteMode;
	if (strcmp(name,"FixedModes") == 0) return FixedModes;
	// Buffer
	if (strcmp(name,"Point0") == 0) return Point0;
	if (strcmp(name,"Versor") == 0) return Versor;
	if (strcmp(name,"Point1") == 0) return Point1;
	if (strcmp(name,"Normal") == 0) return Normal;
	if (strcmp(name,"Coordinate") == 0) return Coordinate;
	if (strcmp(name,"Color") == 0) return Color;
	if (strcmp(name,"Weight") == 0) return Weight;
	if (strcmp(name,"Tag") == 0) return Tag;
	if (strcmp(name,"Point2") == 0) return Point2;
	if (strcmp(name,"Face1") == 0) return Face1;
	if (strcmp(name,"Triple0") == 0) return Triple0;
	if (strcmp(name,"Triple1") == 0) return Triple1;
	if (strcmp(name,"Feedback") == 0) return Feedback;
	if (strcmp(name,"Uniform") == 0) return Uniform;
	if (strcmp(name,"Inquery") == 0) return Inquery;
	if (strcmp(name,"Texture0") == 0) return Texture0;
	if (strcmp(name,"Texture1") == 0) return Texture1;
	if (strcmp(name,"Buffers") == 0) return Buffers;
	// Program
    if (strcmp(name,"Draw") == 0) return Draw;
    if (strcmp(name,"Pierce") == 0) return Pierce;
    if (strcmp(name,"Sect0") == 0) return Sect0;
    if (strcmp(name,"Sect1") == 0) return Sect1;
    if (strcmp(name,"Side1") == 0) return Side1;
    if (strcmp(name,"Side2") == 0) return Side2;
    if (strcmp(name,"Programs") == 0) return Programs;
	// Configure
	if (strcmp(name,"InflateConf") == 0) return InflateConf;
	if (strcmp(name,"SpaceConf") == 0) return SpaceConf;
	if (strcmp(name,"RegionConf") == 0) return RegionConf;
	if (strcmp(name,"PlaneConf") == 0) return PlaneConf;
	if (strcmp(name,"PictureConf") == 0) return PictureConf;
	if (strcmp(name,"OnceConf") == 0) return OnceConf;
	if (strcmp(name,"NotifyConf") == 0) return NotifyConf;
	if (strcmp(name,"RelativeConf") == 0) return RelativeConf;
	if (strcmp(name,"AbsoluteConf") == 0) return AbsoluteConf;
	if (strcmp(name,"RefineConf") == 0) return RefineConf;
	if (strcmp(name,"ManipConf") == 0) return ManipConf;
	if (strcmp(name,"PressConf") == 0) return PressConf;
	if (strcmp(name,"ClickConf") == 0) return ClickConf;	
	if (strcmp(name,"AdditiveConf") == 0) return AdditiveConf;
	if (strcmp(name,"SubtractiveConf") == 0) return SubtractiveConf;
	if (strcmp(name,"Configures") == 0) return Configures;
	// Function
	if (strcmp(name,"AttachedFunc") == 0) return AttachedFunc;
	if (strcmp(name,"Functions") == 0) return Functions;
	// Event
	if (strcmp(name,"StartEvent") == 0) return StartEvent;
	if (strcmp(name,"StopEvent") == 0) return StopEvent;
	if (strcmp(name,"SoundEvent") == 0) return SoundEvent;
	if (strcmp(name,"OnceEvent") == 0) return OnceEvent;
	if (strcmp(name,"NotifyEvent") == 0) return NotifyEvent;
	if (strcmp(name,"UpdateEvent") == 0) return UpdateEvent;
	if (strcmp(name,"Events") == 0) return Events;
	// Factor
	if (strcmp(name,"ConstFactor") == 0) return ConstFactor;
	if (strcmp(name,"VaryFactor") == 0) return VaryFactor;
	if (strcmp(name,"SquareFactor") == 0) return SquareFactor;
	if (strcmp(name,"CompFactor") == 0) return CompFactor;
	if (strcmp(name,"Factors") == 0) return Factors;
	// Change
	if (strcmp(name,"SculptChange") == 0) return SculptChange;
	if (strcmp(name,"GlobalChange") == 0) return GlobalChange;
	if (strcmp(name,"MatrixChange") == 0) return MatrixChange;
	if (strcmp(name,"PlaneChange") == 0) return PlaneChange;
	if (strcmp(name,"RegionChange") == 0) return RegionChange;
	if (strcmp(name,"TextChange") == 0) return TextChange;
	if (strcmp(name,"Changes") == 0) return Changes;
	// Given
	if (strcmp(name,"DoublesGiv") == 0) return DoublesGiv;
	if (strcmp(name,"FloatsGiv") == 0) return FloatsGiv;
	if (strcmp(name,"IntsGiv") == 0) return IntsGiv;
	if (strcmp(name,"CharsGiv") == 0) return CharsGiv;
	if (strcmp(name,"CharGiv") == 0) return CharGiv;
	if (strcmp(name,"IntGiv") == 0) return IntGiv;
	if (strcmp(name,"Givens") == 0) return Givens;
	fatal("unknown name",0,__FILE__,__LINE__);
	return -1;
}

void *rdPointer(int fd)
{
	void *ptr;
	if (read(fd,&ptr,sizeof(ptr)) != sizeof(ptr)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdPointer: %d %p\n",getpid(),ptr);
	return ptr;
}

int rdOpcode(int fd)
{
	enum Opcode opcode; int ret;
	if ((ret = read(fd,&opcode,sizeof(opcode))) < 0) error("read failed",errno,__FILE__,__LINE__);
	if (ret == 0) exit(0);
	if (DEBUG & HASKELL_DEBUG) printf("rdOpcode: %d %s\n",getpid(),enamerate(opcode));
	return opcode;
}

int rdSource(int fd)
{
	enum Source source;
	if (read(fd,&source,sizeof(source)) != sizeof(source)) error ("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdSource: %d %d\n",getpid(),source);
	return source;
}

int rdSubconf(int fd)
{
	enum Subconf subconf;
	if (read(fd,&subconf,sizeof(subconf)) != sizeof(subconf)) error ("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdSource: %d %d\n",getpid(),subconf);
	return subconf;
}

int rdSculpt(int fd)
{
	enum Sculpt sculpt;
	if (read(fd,&sculpt,sizeof(sculpt)) != sizeof(sculpt)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdSculpt: %d %d\n",getpid(),sculpt);
	return sculpt;
}

int rdClickMode(int fd)
{
	enum ClickMode clickMode;
	if (read(fd,&clickMode,sizeof(clickMode)) != sizeof(clickMode)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdClickMode: %d %d\n",getpid(),clickMode);
	return clickMode;
}

int rdMouseMode(int fd)
{
	enum MouseMode mouseMode;
	if (read(fd,&mouseMode,sizeof(mouseMode)) != sizeof(mouseMode)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdMouseMode: %d %d\n",getpid(),mouseMode);
	return mouseMode;
}

int rdRollerMode(int fd)
{
	enum RollerMode rollerMode;
	if (read(fd,&rollerMode,sizeof(rollerMode)) != sizeof(rollerMode)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdRollerMode: %d %d\n",getpid(),rollerMode);
	return rollerMode;
}

int rdTargetMode(int fd)
{
	enum TargetMode targetMode;
	if (read(fd,&targetMode,sizeof(targetMode)) != sizeof(targetMode)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdTargetMode: %d %d\n",getpid(),targetMode);
	return targetMode;
}

int rdTopologyMode(int fd)
{
	enum TopologyMode topologyMode;
	if (read(fd,&topologyMode,sizeof(topologyMode)) != sizeof(topologyMode)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdTopologyMode: %d %d\n",getpid(),topologyMode);
	return topologyMode;
}

int rdFixedMode(int fd)
{
	enum FixedMode fixedMode;
	if (read(fd,&fixedMode,sizeof(fixedMode)) != sizeof(fixedMode)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdFixedMode: %d %d\n",getpid(),fixedMode);
	return fixedMode;
}

int rdBuffer(int fd)
{
	enum Buffer buffer;
	if (read(fd,&buffer,sizeof(buffer)) != sizeof(buffer)) error ("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdSource: %d %d\n",getpid(),buffer);
	return buffer;
}

int rdProgram(int fd)
{
	enum Program program;
	if (read(fd,&program,sizeof(program)) != sizeof(program)) error ("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdSource: %d %d\n",getpid(),program);
	return program;
}

int rdConfigure(int fd)
{
	enum Configure configure;
	if (read(fd,&configure,sizeof(configure)) != sizeof(configure)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdConfigure: %d %d\n",getpid(),configure);
	return configure;
}

int rdFunction(int fd)
{
	enum Function function;
	if (read(fd,&function,sizeof(function)) != sizeof(function)) error ("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdSource: %d %d\n",getpid(),function);
	return function;
}

int rdEvent(int fd)
{
	enum Event event;
	if (read(fd,&event,sizeof(event)) != sizeof(event)) error ("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdSource: %d %d\n",getpid(),event);
	return event;
}

int rdFactor(int fd)
{
	enum Factor factor;
	if (read(fd,&factor,sizeof(factor)) != sizeof(factor)) error ("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdSource: %d %d\n",getpid(),factor);
	return factor;
}

int rdChange(int fd)
{
	enum Change change;
	if (read(fd,&change,sizeof(change)) != sizeof(change)) error ("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdSource: %d %d\n",getpid(),change);
	return change;
}

int rdGiven(int fd)
{
	enum Given given;
	if (read(fd,&given,sizeof(given)) != sizeof(given)) error ("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdSource: %d %d\n",getpid(),given);
	return given;
}

char rdChar(int fd)
{
	char val;
	if (read(fd,&val,sizeof(val)) != sizeof(val)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdChar: %d %c\n",getpid(),val);
	return val;
}

int rdInt(int fd)
{
	int val;
	if (read(fd,&val,sizeof(val)) != sizeof(val)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdInt: %d %d\n",getpid(),val);
	return val;
}

float rdFloat(int fd)
{
	float val;
	if (read(fd,&val,sizeof(val)) != sizeof(val)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdFloat: %d %f\n",getpid(),val);
	return val;
}

double rdDouble(int fd)
{
	double val;
	if (read(fd,&val,sizeof(val)) != sizeof(val)) error("read failed",errno,__FILE__,__LINE__);
	if (DEBUG & HASKELL_DEBUG) printf("rdDouble: %d %f\n",getpid(),val);
	return val;
}

void wrPointer(int fd, void *ptr)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrPointer %d %p\n",getpid(),ptr);
	if (write(fd,&ptr,sizeof(ptr)) != sizeof(ptr)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrOpcode(int fd, int val)
{
	enum Opcode opcode = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrOpcode: %d %s\n",getpid(),enamerate(opcode));
	if (write(fd,&opcode,sizeof(opcode)) != sizeof(opcode)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrSource(int fd, int val)
{
	enum Source source = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrSource %d %d\n",getpid(),val);
	if (write(fd,&source,sizeof(source)) != sizeof(source)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrSubconf(int fd, int val)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrSubconf %d %d\n",getpid(),val);
	enum Subconf subconf = val;
	if (write(fd,&subconf,sizeof(subconf)) != sizeof(subconf)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrSculpt(int fd, int val)
{
	enum Sculpt sculpt = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrSculpt %d %d\n",getpid(),val);
	if (write(fd,&sculpt,sizeof(sculpt)) != sizeof(sculpt)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrClickMode(int fd, int val)
{
	enum ClickMode clickMode = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrClickMode %d %d\n",getpid(),val);
	if (write(fd,&clickMode,sizeof(clickMode)) != sizeof(clickMode)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrMouseMode(int fd, int val)
{
	enum MouseMode mouseMode = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrMouseMode %d %d\n",getpid(),val);
	if (write(fd,&mouseMode,sizeof(mouseMode)) != sizeof(mouseMode)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrRollerMode(int fd, int val)
{
	enum RollerMode rollerMode = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrRollerMode %d %d\n",getpid(),val);
	if (write(fd,&rollerMode,sizeof(rollerMode)) != sizeof(rollerMode)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrTargetMode(int fd, int val)
{
	enum TargetMode targetMode = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrTargetMode %d %d\n",getpid(),val);
	if (write(fd,&targetMode,sizeof(targetMode)) != sizeof(targetMode)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrTopologyMode(int fd, int val)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrTopologyMode %d %d\n",getpid(),val);
	enum TopologyMode topologyMode = val;
	if (write(fd,&topologyMode,sizeof(topologyMode)) != sizeof(topologyMode)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrFixedMode(int fd, int val)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrFixedMode %d %d\n",getpid(),val);
	enum FixedMode fixedMode = val;
	if (write(fd,&fixedMode,sizeof(fixedMode)) != sizeof(fixedMode)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrBuffer(int fd, int val)
{
	enum Buffer buffer = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrBuffer %d %d\n",getpid(),val);
	if (write(fd,&buffer,sizeof(buffer)) != sizeof(buffer)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrProgram(int fd, int val)
{
	enum Program program = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrProgram %d %d\n",getpid(),val);
	if (write(fd,&program,sizeof(program)) != sizeof(program)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrConfigure(int fd, int val)
{
	enum Configure configure = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrConfigure %d %d\n",getpid(),val);
	if (write(fd,&configure,sizeof(configure)) != sizeof(configure)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrFunction(int fd, int val)
{
	enum Function function = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrFactor %d %d\n",getpid(),val);
	if (write(fd,&function,sizeof(function)) != sizeof(function)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrEvent(int fd, int val)
{
	enum Event event = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrEvent %d %d\n",getpid(),val);
	if (write(fd,&event,sizeof(event)) != sizeof(event)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrFactor(int fd, int val)
{
	enum Factor factor = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrFactor %d %d\n",getpid(),val);
	if (write(fd,&factor,sizeof(factor)) != sizeof(factor)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrChange(int fd, int val)
{
	enum Change change = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrChange %d %d\n",getpid(),val);
	if (write(fd,&change,sizeof(change)) != sizeof(change)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrGiven(int fd, int val)
{
	enum Given given = val;
	if (DEBUG & HASKELL_DEBUG) printf("wrChange %d %d\n",getpid(),val);
	if (write(fd,&given,sizeof(given)) != sizeof(given)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrChar(int fd, char val)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrChar %d %c\n",getpid(),val);
	if (write(fd,&val,sizeof(val)) != sizeof(val)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrInt(int fd, int val)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrInt %d %d\n",getpid(),val);
	if (write(fd,&val,sizeof(val)) != sizeof(val)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrFloat(int fd, float val)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrFloat %d %f\n",getpid(),val);
	if (write(fd,&val,sizeof(val)) != sizeof(val)) fatal("write failed",errno,__FILE__,__LINE__);
}

void wrDouble(int fd, double val)
{
	if (DEBUG & HASKELL_DEBUG) printf("wrDouble %d %f\n",getpid(),val);
	if (write(fd,&val,sizeof(val)) != sizeof(val)) fatal("write failed",errno,__FILE__,__LINE__);
}

void exOpcode(int fd, int opcode)
{
	if (DEBUG & HASKELL_DEBUG) printf("exOpcode %d %s\n",getpid(),enamerate(opcode));
	if (rdOpcode(fd) != opcode) fatal("unexpected opcode",errno,__FILE__,__LINE__);
}

