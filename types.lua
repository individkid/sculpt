#!/usr/bin/env lua
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

function allexcept(enum,set)
	all = {}
	for key,val in pairs(enum) do all[val]=true end
	for key,val in pairs(set) do all[key]=nil end
	return all
end
function allbefore(enum,lim)
	all = {}
	for key,val in pairs(enum) do if val == lim then break end all[val]=true end
	return all
end
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
Format = {
	{"cursor","float",{},{2}},
	{"affine","float",{},{16}},
	{"perplane","float",{},{16}},
	{"basis","float",{},{9,3}},
	{"cutoff","float",{},{}},
	{"slope","float",{},{}},
	{"aspect","float",{},{}},
	{"feather","float",{},{3}},
	{"arrow","float",{},{3}},
	{"enable","MYuint",{},{}},
	{"filler1","char",{},{0}},
	{"tagplane","MYuint",{},{}},
	{"filler2","char",{},{0}},
	{"taggraph","MYuint",{},{}},
}
Feedback = {
	{"pierce","float",{},{3}},
	{"normal","float",{},{3}},
	{"tagbits","int",{},{}},
	{"plane","int",{},{}},
}
Update = {
	{"next","Update",{},1},
	{"file","int",{},{}},
	{"finish","int",{},{}},
	{"buffer","Buffer",{},{}},
	{"offset","int",{["buffer"]=allexcept(Buffer,{["Texture0"]=true,["Texture1"]=true})},{}},
	{"width","int",{["buffer"]={["Texture0"]=true,["Texture1"]=true}},{}},
	{"size","int",{["buffer"]=allexcept(Buffer,{["Texture0"]=true,["Texture1"]=true})},{}},
	{"height","int",{["buffer"]={["Texture0"]=true,["Texture1"]=true}},{}},
	{"format","Format",{["buffer"]={["Update"]=true}},1},
	{"feedback","Feedback",{["buffer"]={["Feedback"]=true}},1},
	{"query","MYuint*",{["buffer"]={["Inquery"]=true}},{}},
	{"data","char",{["buffer"]=allbefore(Buffer,"Texture0")},"size"},
	{"function","MYfunc*",{},{}},
}
Render = {
	{"next","Render",{},1},
	{"file","int",{},{}},
	{"program","Program",{},{}},
	{"base","int",{},{}},
	{"count","int",{},{}},
	{"size","int",{},{}},
}
Command = {
	{"next","Command",{},1},
	{"file","int",{},{}},
	{"source","Source",{},{}},
	{"subconf","Subconf",{["source"]={["ConfigureSrc"]=true}},{}},
	{"setting","float",{["source"]={["ConfigureSrc"]=true}},{}},
	{"sculpt","Sculpt",{["source"]={["ModeSrc"]=true}},{}},
	{"click","ClickMode",{["source"]={["ModeSrc"]=true},["sculpt"]={["ClickUlpt"]=true}},{}},
	{"mouse","MouseMode",{["source"]={["ModeSrc"]=true},["sculpt"]={["MouseUlpt"]=true}},{}},
	{"roller","RollerMode",{["source"]={["ModeSrc"]=true},["sculpt"]={["RollerUlpt"]=true}},{}},
	{"target","TargetMode",{["source"]={["ModeSrc"]=true},["sculpt"]={["TargetUlpt"]=true}},{}},
	{"topology","TopologyMode",{["source"]={["ModeSrc"]=true},["sculpt"]={["TopologyUlpt"]=true}},{}},
	{"fixed","FixedMode",{["source"]={["ModeSrc"]=true},["sculpt"]={["FixedUlpt"]=true}},{}},
	{"matrix","float",{["source"]={["MatrixSrc"]=true,["GlobalSrc"]=true}},16},
	{"feedback","int",{["source"]={["PolytopeSrc"]=true}},{}},
	{"finish","int",{["source"]={["PolytopeSrc"]=true}},{}},
	{"update","Update",{["source"]={["PolytopeSrc"]=true}},"Field"},
	{"render","Render",{["source"]={["PolytopeSrc"]=true}},1},
	{"redraw","Command",{["source"]={["PolytopeSrc"]=true}},1},
	{"pierce","Command",{["source"]={["PolytopeSrc"]=true}},1},
	{"plane","int",{["source"]={["MacroSrc"]=true}},{}},
	{"key","char",{["source"]={["HotkeySrc"]=true}},{}},
	{"script","char",{["source"]={["MacroSrc"]=true,["HotkeySrc"]=true}},0},
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
Function = {
	"AttachedFunc",
	"Functions",
}
Data = {
	{"next","Data",{},1},
	{"file","int",{},{}},
	{"plane","int",{},{}},
	{"conf","Configure",{},{}},
	{"boundaries","int",{["conf"]={["SpaceConf"]=true}},{}},
	{"regions","int",{["conf"]={["SpaceConf"]=true}},{}},
	{"planes","int",{["conf"]={["SpaceConf"]=true}},"boundaries"},
	{"sides","int",{["conf"]={["SpaceConf"]=true}},{"boundaries","regions"}},
	{"side","int",{["conf"]={["RegionConf"]=true}},{}},
	{"insides","int",{["conf"]={["RegionConf"]=true}},{}},
	{"outsides","int",{["conf"]={["RegionConf"]=true}},{}},
	{"inside","int",{["conf"]={["RegionConf"]=true}},"insides"},
	{"outside","int",{["conf"]={["RegionConf"]=true}},"outsides"},
	{"versor","int",{["conf"]={["PlaneConf"]=true}},{}},
	{"vector","float",{["conf"]={["PlaneConf"]=true}},3},
	{"filename","char",{["conf"]={["PictureConf"]=true}},0},
	{"func","Function",{["conf"]={["OnceConf"]=true,["NotifyConf"]=true}},{}},
	{"count","int",{["conf"]={["OnceConf"]=true,["NotifyConf"]=true}},{}},
	{"specify","int",{["conf"]={["OnceConf"]=true,["NotifyConf"]=true}},"count"},
	{"script","char",{["conf"]={["OnceConf"]=true,["NotifyConf"]=true}},0},
	{"fixed","float",{["conf"]={["RelativeConf"]=true}},3},
	{"relative","TopologyMode",{["conf"]={"RelativeConf"}},{}},
	{"absolute","TopologyMode",{["conf"]={"AbsoluteConf"}},{}},
	{"pierce","float",{["conf"]={["RefineConf"]=true}},3},
	{"matrix","float",{["conf"]={["ManipConf"]=true}},16},
	{"press","char",{["conf"]={["PressConf"]=true}},{}},
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
	{"coef","double",{},{}},
	{"factor","Factor",{},{}},
	{"id","int",{[1]=1},"factor"},
	{"ptr","double*",{[1]=2},"factor"},
}
Sum = {
	{"count","int",{},{}},
	{"term","Term",{},"count"},
}
Equ = {
	{"numer","Sum",{},{}},
	{"denom","Sum",{},{}},
}
Sound = {
	{"next","Sound",{},1},
	{"done","int",{},{}},
	{"ident","int",{},{}},
	{"value","double",{},{}},
	{"event","Event",{},{}},
	{"equ","Equ",{["event"]={["SoundEvent"]=true}},"Equate"},
	{"sched","Equ",{["event"]={["OnceEvent"]=true,["NotifyEvent"]=true}},{}},
	{"count","int",{["event"]={["OnceEvent"]=true,["NotifyEvent"]=true}},{}},
	{"ids","int",{["event"]={["OnceEvent"]=true,["NotifyEvent"]=true},[1]=1},"count"},
	{"ptrs","double*",{["event"]={["OnceEvent"]=true,["NotifyEvent"]=true},[1]=2},"count"},
	{"script","char",{["event"]={["OnceEvent"]=true,["NotifyEvent"]=true}},0},
	{"id","int",{["event"]={["UpdateEvent"]=true}},{}},
	{"update","double*",{["event"]={["UpdateEvent"]=true}},{}},
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
	{"file","int",{},{}},
	{"plane","int",{},{}},
	{"change","Change",{},{}},
	{"sculpt","Sculpt",{["change"]={["SculptChange"]=true}},{}},
	{"click","ClickMode",{["change"]={["SculptChange"]=true},["sculpt"]={["ClickUlpt"]=true}},{}},
	{"mouse","MouseMode",{["change"]={["SculptChange"]=true},["sculpt"]={["MouseUlpt"]=true}},{}},
	{"roller","RollerMode",{["change"]={["SculptChange"]=true},["sculpt"]={["RollerUlpt"]=true}},{}},
	{"target","TargetMode",{["change"]={["SculptChange"]=true},["sculpt"]={["TargetUlpt"]=true}},{}},
	{"topology","TopologyMode",{["change"]={["SculptChange"]=true},["sculpt"]={["TopologyUlpt"]=true}},{}},
	{"fixed","FixedMode",{["change"]={["SculptChange"]=true},["sculpt"]={["FixedUlpt"]=true}},{}},
	{"side","int",{["change"]={["RegionChange"]=true}},{}},
	{"insides","int",{["change"]={["RegionChange"]=true}},{}},
	{"outsides","int",{["change"]={["RegionChange"]=true}},{}},
	{"inside","int",{["change"]={["RegionChange"]=true}},"insides"},
	{"outside","int",{["change"]={["RegionChange"]=true}},"outsides"},
	{"matrix","float",{["change"]={["MatrixChange"]=true,["GlobalChange"]=true}},16},
	{"versor","int",{["change"]={["PlaneChange"]=true}},{}},
	{"vector","float",{["change"]={["PlaneChange"]=true}},3},
	{"text","char",{["change"]={["TextChange"]=true}},0},
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
	{"given","Given",{},{}},
	{"file","int",{["given"]={["IntGiv"]=true}},{}},
	{"length","int",{["given"]=allexcept(Given,{["CharGiv"]=true,["IntGiv"]=true})},{}},
	{"doubles","double",{["given"]={["DoublesGiv"]=true}},"length"},
	{"floats","float",{["given"]={["FloatsGiv"]=true}},"length"},
	{"ints","int",{["given"]={["IntsGiv"]=true}},"length"},
	{"chars","char",{["given"]={["CharsGiv"]=true}},"length"},
	{"key","char",{["given"]={["CharGiv"]=true}},{}},
	{"plane","int",{["given"]={["IntGiv"]=true}},{}},
}
Enums = {["Event"]=true,["Equate"]=true,["Factor"]=true,["Change"]=true,["Given"]=true,["Configure"]=true,["Function"]=true}
Structs = {["Term"]=true,["Sum"]=true,["Equ"]=true,["Sound"]=true,["State"]=true,["Query"]=true,["Data"]=true}
function printstruct(name,struct)
	print("struct "..name.." {")
	for key,val in pairs(struct) do
		if (Enums[val[2]]~=nil) then
			decl = "enum "..val[2]
		elseif (Structs[val[2]]~=nil) then
			decl = "struct "..val[2]
		else
			decl = val[2]
		end
		if (type(val[4]) == "number") then
			decl = decl.."*"
		elseif (type(val[4]) == "string") then
			decl = decl.."*"
		end
		ident = val[1]
		if (type(val[4]) == "table") then
			for k,v in pairs(val[4]) do
				ident = ident.."["..v.."]"
			end
		end
		print("    "..decl.." "..ident..";")
	end
print("};")
end
printstruct("Query",Query)
