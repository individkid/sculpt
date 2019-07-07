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
-- {name,type,{field={values}},nil(field)|{int}(array)|{}(opaque)|int(alloc)|0(string)|Enum(array)|field(array)}
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
	{"query","MYuint*",{"buffer"={"Inquery"=true}},nil},
	{"data","char",{"buffer"=allbefore(Buffer,"Texture0")},"size"},
	{"function","MYfunc*",{},nil},
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
Data = {
	{"next","Data",{},1},
	{"file","int",{},nil},
	{"plane","int",{},nil},
	{"conf","Configure",{},nil},
	{"boundaries","int",{"conf"={"SpaceConf"=true}},nil},
	{"regions","int",{"conf"={"SpaceConf"=true}},nil},
	{"planes","int",{"conf"={"SpaceConf"=true}},"boundaries"},
	{"sides","int",{"conf"={"SpaceConf"=true}},{"boundaries","regions"}},
	{"side","int",{"conf"={"RegionConf"=true}},nil},
	{"insides","int",{"conf"={"RegionConf"=true}},nil},
	{"outsides","int",{"conf"={"RegionConf"=true}},nil},
	{"inside","int",{"conf"={"RegionConf"=true}},"insides"},
	{"outside","int",{"conf"={"RegionConf"=true}},"outsides"},
	{"versor","int",{"conf"={"PlaneConf"=true}},nil},
	{"vector","float",{"conf"={"PlaneConf"=true}},3},
	{"filename","char",{"conf"={"PictureConf"=true}},0},
	{"func","Function",{"conf"={"OnceConf"=true,"NotifyConf"=true}},nil},
	{"count","int",{"conf"={"OnceConf"=true,"NotifyConf"=true}},nil},
	{"specify","int",{"conf"={"OnceConf"=true,"NotifyConf"=true}},"count"},
	{"script","char",{"conf"={"OnceConf"=true,"NotifyConf"=true}},0},
	{"fixed","float",{"conf"={"RelativeConf"=true}},3},
	{"relative","TopologyMode",{"conf"={"RelativeConf"}},nil},
	{"absolute","TopologyMode",{"conf"={"AbsoluteConf"}},nil},
	{"pierce","float",{"conf"={"RefineConf"=true}},3},
	{"matrix","float",{"conf"={"ManipConf"=true}},16},
	{"press","char",{"conf"={"PressConf"=true}},nil},
}
Event = {
	"StartEvent",
	"StopEvent",
	"SoundEvent",
	"OnceEvent",
	"NotifyEvent",
	"UpdateEvent",
	"Events",
}
Equate = {
	"ValueEqu",
	"DelayEqu",
	"SchedEqu",
	"LeftEqu",
	"RightEqu",
	"Equates",
}
Factor = {
	"ConstFactor",
	"VaryFactor",
	"SquareFactor",
	"CompFactor",
	"Factors",
}
Term = {
	{"coef","double",{},nil},
	{"factor","Factor",{},nil},
	{"id","int",{0=1},"factor"},
	{"ptr","double*",{0=2},"factor")},
}
Sum = {
	{"count","int",{},nil},
	{"term","Term",{},"count"},
}
Equ = {
	{"numer","Sum",{},nil},
	{"denom","Sum",{},nil},
}
Sound = {
	{"next","Sound",{},1},
	{"done","int",{},nil},
	{"ident","int",{},nil},
	{"value","double",{},nil},
	{"event","Event",{},nil},
	{"equ","Equ",{"event"={"SoundEvent"=true}},"Equate"},
	{"sched","Equ",{"event"={"OnceEvent"=true,"NotifyEvent"=true}},nil},
	{"count","int",{"event"={"OnceEvent"=true,"NotifyEvent"=true}},nil},
	{"ids","int",{"event"={"OnceEvent"=true,"NotifyEvent"=true},0=1},"count"},
	{"ptrs","double*",{"event"={"OnceEvent"=true,"NotifyEvent"=true},0=2},"count"},
	{"script","char",{"event"={"OnceEvent"=true,"NotifyEvent"=true}},0},
	{"id","int",{"event"={"UpdateEvent"=true}},nil},
	{"update","double*",{"event"={"UpdateEvent"=true}},nil},
}
Change = {
	"SculptChange",
	"GlobalChange",
	"MatrixChange",
	"PlaneChange",
	"RegionChange",
	"TextChange",
	"Changes",
}
State = {
	{"next","State",{},1},
	{"file","int",{},nil},
	{"plane","int",{},nil},
	{"change","Change",{},nil},
	{"sculpt","Sculpt",{"change"={"SculptChange"=true}},nil},
	{"click","ClickMode",{"change"={"SculptChange"=true},"sculpt"={"ClickUlpt"=true}},nil},
	{"mouse","MouseMode",{"change"={"SculptChange"=true},"sculpt"={"MouseUlpt"=true}},nil},
	{"roller","RollerMode",{"change"={"SculptChange"=true},"sculpt"={"RollerUlpt"=true}},nil},
	{"target","TargetMode",{"change"={"SculptChange"=true},"sculpt"={"TargetUlpt"=true}},nil},
	{"topology","TopologyMode",{"change"={"SculptChange"=true},"sculpt"={"TopologyUlpt"=true}},nil},
	{"fixed","FixedMode",{"change"={"SculptChange"=true},"sculpt"={"FixedUlpt"=true}},nil},
	{"side","int",{"change"={"RegionChange"=true}},nil},
	{"insides","int",{"change"={"RegionChange"=true}},nil},
	{"outsides","int",{"change"={"RegionChange"=true}},nil},
	{"inside","int",{"change"={"RegionChange"=true}},"insides"},
	{"outside","int",{"change"={"RegionChange"=true}},"outsides"},
	{"matrix","float",{"change"={"MatrixChange"=true,"GlobalChange"=true}}}
	{"versor","int",{"change"={"PlaneChange"=true}},nil},
	{"vector","float",{"change"={"PlaneChange"=true}},3},
	{"text","char",{"change"={"TextChange"=true}},0},
}
Given = {
	"DoublesGiv",
	"FloatsGiv",
	"IntsGiv",
	"CharsGiv",
	"CharGiv",
	"IntGiv",
	"Givens",
}
Query = {
	{"next","Query",{},1},
	{"script","char",{},0},
	{"given","Given",{},nil},
	{"file","int",{"given"={"IntGiv"=true}},nil},
	{"length","int",{"given"=allexcept({"CharGiv"=true,"IntGiv"=true})},nil},
	{"doubles","double",{"given"={"DoublesGiv"=true}},"length"},
	{"floats","float",{"given"={"FloatsGiv"=true}},"length"},
	{"ints","int",{"given"={"IntsGiv"=true}},"length"},
	{"chars","char",{"given"={"CharsGiv"=true}},"length"},
	{"key","char",{"given"={"CharGiv"=true}},nil},
	{"plane","int",{"given"={"IntGiv"=true}},nil},
}
