--[[
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
--]]
Source = {
	"ConfigureSrc",
	"ModeSrc",
	"MatrixSrc",
	"GlobalSrc",
	"PolytopeSrc",
	"MacroSrc",
	"HotkeySrc",
	"Sources",
}
Subconf = {
	"StartSub",
	"StopSub",
	"Subconfs",
}
Sculpt = {
	"ClickUlpt",
	"MouseUlpt",
	"RollerUlpt",
	"TargetUlpt",
	"TopologyUlpt",
	"FixedUlpt",
	"Sculpts",
}
ClickMode = {
	"AdditiveMode",
	"SubtractiveMode",
	"RefineMode",
	"TweakMode",
	"PerformMode",
	"TransformMode",
	"SuspendMode",
	"PierceMode",
	"ClickModes",
}
MouseMode = {
	"RotateMode",
	"TangentMode",
	"TranslateMode",
	"MouseModes",
}
RollerMode = {
	"CylinderMode",
	"ClockMode",
	"NormalMode",
	"ParallelMode",
	"ScaleMode",
	"RollerModes",
}
TargetMode = {
	"SessionMode",
	"PolytopeMode",
	"FacetMode",
	"TargetModes",
}
TopologyMode = {
	"NumericMode",
	"InvariantMode",
	"SymbolicMode",
	"TopologyModes",
}
FixedMode = {
	"RelativeMode",
	"AbsoluteMode",
	"FixedModes",
}
Field = {
	"AllocField",
	"WriteField",
	"BindField",
	"ReadField",
	"Fields",
}
Buffer = {
	"Point0",
	"Versor",
	"Point1",
	"Normal",
	"Coordinate",
	"Color",
	"Weight",
	"Tag",
	"Point2",
	"Face1",
	"Triple0",
	"Triple1",
	"Texture0",
	"Texture1",
	"Uniform",
	"Feedback",
	"Inquery",
	"Buffers",
}
Program = {
	"Draw",
	"Pierce",
	"Sect0",
	"Sect1",
	"Side1",
	"Side2",
	"Programs",
}
-- {name,type,{field={values}},nil(field)|{int}(array)|{}(pointer)|int(alloc)|Enum(array)|field(array)}
Format = {
	{"cursor","float",{},{2}},
	{"affine","float",{},{16}},
	{"perplane","float",{},{16}},
	{"basis","float",{},{9,3}},
	{"cutoff","float",{},nil},
	{"slope","float",{},nil},
	{"aspect","float",{},nil},
	{"feather","float",{},{3}},
	{"arrow","float",{},{3}},
	{"enable","MYuint",{},nil},
	{"filler1","char",{},{0}},
	{"tagplane","MYuint",{},nil},
	{"filler2","char",{},{0}},
	{"taggraph","MYuint",{},nil},
}
Feedback = {
	{"pierce","float",{},{3}},
	{"normal","float",{},{3}},
	{"tagbits","int",{},nil},
	{"plane","int",{},nil},
}
Update = {
	{"next","Update",{},1},
	{"file","int",{},nil},
	{"finish","int",{},nil},
	{"buffer","Buffer",{},nil},
	{"offset","int",{"buffer"=allexcept(Buffer,{"Texture0"=true,"Texture1"=true})},nil},
	{"width","int",{"buffer"={"Texture0"=true,"Texture1"=true}},nil},
	{"size","int",{"buffer"=allexcept(Buffer,{"Texture0"=true,"Texture1"=true})},nil},
	{"height","int",{"buffer"={"Texture0"=true,"Texture1"=true}},nil},
	{"format","Format",{"buffer"={"Update"=true}},1},
	{"feedback","Feedback",{"buffer"={"Feedback"=true}},1},
	{"query","MYuint",{"buffer"={"Inquery"=true}},{}},
	{"data","char",{"buffer"=allbefore(Buffer,"Texture0")},"size"},
	{"function","MYfunc",{},{}},
}
Render = {
	{"next","Render",{},1},
	{"file","int",{},nil},
	{"program","Program",{},nil},
	{"base","int",{},nil},
	{"count","int",{},nil},
	{"size","int",{},nil},
}
Command = {
	{"next","Command",{},1},
	{"file","int",{},nil},
	{"source","Source",{},nil},
	{"subconf","Subconf",{"source"={"ConfigureSrc"=true}},nil},
	{"setting","float",{"source"={"ConfigureSrc"=true}},nil},
	{"sculpt","Sculpt",{"source"={"ModeSrc"=true}},nil},
	{"click","ClickMode",{"source"={"ModeSrc"=true},"sculpt"={"ClickUlpt"=true}},nil},
	{"mouse","MouseMode",{"source"={"ModeSrc"=true},"sculpt"={"MouseUlpt"=true}},nil},
	{"roller","RollerMode",{"source"={"ModeSrc"=true},"sculpt"={"RollerUlpt"=true}},nil},
	{"target","TargetMode",{"source"={"ModeSrc"=true},"sculpt"={"TargetUlpt"=true}},nil},
	{"topology","TopologyMode",{"source"={"ModeSrc"=true},"sculpt"={"TopologyUlpt"=true}},nil},
	{"fixed","FixedMode",{"source"={"ModeSrc"=true},"sculpt"={"FixedUlpt"=true}},nil},
	{"matrix","float",{"source"={"MatrixSrc"=true,"GlobalSrc"=true}},16},
	{"feedback","int",{"source"={"PolytopeSrc"=true}},nil},
	{"finish","int",{"source"={"PolytopeSrc"=true}},nil},
	{"update","Update",{"source"={"PolytopeSrc"=true}},"Field"},
	{"render","Render",{"source"={"PolytopeSrc"=true}},1},
	{"redraw","Command",{"source"={"PolytopeSrc"=true}},1},
	{"pierce","Command",{"source"={"PolytopeSrc"=true}},1},
	{"plane","int",{"source"={"MacroSrc"=true}},nil},
	{"key","char",{"source"={"HotkeySrc"=true}},nil},
	{"script","char",{"source"={"MacroSrc"=true,"HotkeySrc"=true}},0},
}
Configure = {
	"InflateConf",
	"SpaceConf",
	"RegionConf",
	"PlaneConf",
	"PictureConf",
	"OnceConf",
	"NotifyConf",
	"RelativeConf",
	"AbsoluteConf",
	"RefineConf",
	"ManipConf",
	"PressConf",
	"ClickConf",
	"AdditiveConf",
	"SubtractiveConf",
	"Configures",
}
enum Function {
	"AttachedFunc",
	"Functions",
}
struct Data
{
	{"next","Data",{},1},
	{"file","int",{},nil},
	{"plane","int",{},nil},
	{"conf","Configure",{},nil},
	{"boundaries","int",{"conf"={"SpaceConf"=true}},nil},
	{"regions","int",{"conf"={"SpaceConf"=true}},nil},
	{"planes","int",{"conf"={"SpaceConf"=true}},"boundaries"},
	{"sides","int",{"conf"={"SpaceConf"=true}},{"boundaries","regions"}},
	// Script->RegionConf->Polytope
	struct {
	{"side","int",{"conf"={"RegionConf"=true}},nil},
	{"insides","int",{"conf"={"RegionConf"=true}},nil},
	{"outsides","int",{"conf"={"RegionConf"=true}},nil},
	{"inside","int",{"conf"={"RegionConf"=true}},"insides"},
	{"outside","int",{"conf"={"RegionConf"=true}},"outsides"},
	{"versor","int",{"conf"={"PlaneConf"=true}},nil},
	{"vector","float",{"conf"={"PlaneConf"=true}},3},
	// Script->PictureConf->Polytope
	char *filename;
	// Script->(OnceConf,NotifyConf)->Polytope
	struct {enum Function func; int count; int *specify; char *script;};
	// (Script,Window)->RelativeConf->Polytope
	struct {float *fixed; enum TopologyMode relative;};
	// (Script,Window)->AbsoluteConf->Polytope
	enum TopologyMode absolute;
	// (Script,Window)->RefineConf->Polytope
	float *pierce;
	// (Script,Window)->ManipConf->Poltope
	float *matrix;
	// (Script,Window)->PressConf->Polytope
	char press;};
	// (Script,Window)->ClickConf->Polytope
	// (Script,Window)->AdditiveConf->Polytope
	// (Script,Window)->SubtractiveConf->Polytope
};
