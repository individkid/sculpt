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

function allOf(enum)
	all = {}
	for key,val in pairs(enum) do all[val]=true end
	return all
end
function allExcept(enum,set)
	all = {}
	for key,val in pairs(enum) do all[val] = not set[key] end
	return all
end
function allBefore(enum,lim)
	all = {}
	for key,val in pairs(enum) do all[val] = val < lim end
	return all
end
function interSet(lhs,rhs)
	result = {}
	for k,v in pairs(lhs) do result[k] = v and rhs[k] end
	return result
end
function differSet(lhs,rhs)
	result = {}
	for k,v in pairs(lhs) do result[k] = v and not rhs[k] end
	return result
end
function unionSet(lhs,rhs)
	result = {}
	for k,v in pairs(lhs) do result[k] = v or rhs[k] end
	for k,v in pairs(rhs) do result[k] = v or lhs[k] end
	return result
end
function unionDimSet(lhs,rhs)
	result = {}
	count = 0
	for k,v in pairs(lhs) do
		if rhs[k] then result[k] = unionSet(v,rhs[k]) end
		if result[k] == {} then count = count + 1 end
	end
	if count+1 == #result then return {} end
	if count > 0 then return {[""]={}} end
	return result
end
function interDimSet(lhs,rhs)
	result = {}
	count = 0
	for k,v in pairs(lhs) do
		if rhs[k] then result[k] = interSet(v,rhs[k]) else result[k] = v end
		if result[k] == {} then count = count + 1 end
	end
	for k,v in pairs(rhs) do
		if lhs[k] then result[k] = interSet(v,lhs[k]) else result[k] = v end
		if result[k] == {} then count = count + 1 end
	end
	if count+1 == #result then return {} end
	if count > 0 then return {[""]={}} end
	return result
end
function differDimSet(lhs,rhs)
	result = {}
	count = 0
	for k,v in pairs(lhs) do
		if rhs[k] then result[k] = differSet(v,rhs[k]) else result[k] = {} end
		if result[k] == {} then count = count + 1 end
	end
	if count+1 == #result then return {} end
	if count > 0 then return {[""]={}} end
	return result
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
	{"offset","int",{["buffer"]=allExcept(Buffer,{["Texture0"]=true,["Texture1"]=true})},{}},
	{"width","int",{["buffer"]={["Texture0"]=true,["Texture1"]=true}},{}},
	{"size","int",{["buffer"]=allExcept(Buffer,{["Texture0"]=true,["Texture1"]=true})},{}},
	{"height","int",{["buffer"]={["Texture0"]=true,["Texture1"]=true}},{}},
	{"format","Format",{["buffer"]={["Update"]=true}},1},
	{"feedback","Feedback",{["buffer"]={["Feedback"]=true}},1},
	{"query","MYuint*",{["buffer"]={["Inquery"]=true}},{}},
	{"data","char",{["buffer"]=allBefore(Buffer,"Texture0")},"size"},
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
Done = {
	"IdentityDone",
	"PointerDone",
	"Dones",
}
Term = {
	{"done","Done",{},{}},
	{"coef","double",{},{}},
	{"factor","Factor",{},{}},
	{"done","Done",{},{}},
	{"id","int",{["done"]={["IdentityDone"]=true}},"factor"},
	{"ptr","double*",{["done"]={["PointerDone"]=true}},"factor"},
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
	{"done","Done",{},{}},
	{"ident","int",{},{}},
	{"value","double",{},{}},
	{"event","Event",{},{}},
	{"equ","Equ",{["event"]={["SoundEvent"]=true}},"Equate"},
	{"sched","Equ",{["event"]={["OnceEvent"]=true,["NotifyEvent"]=true}},{}},
	{"count","int",{["event"]={["OnceEvent"]=true,["NotifyEvent"]=true}},{}},
	{"ids","int",{["event"]={["OnceEvent"]=true,["NotifyEvent"]=true},["done"]={["IdentityDone"]=true}},"count"},
	{"ptrs","double*",{["event"]={["OnceEvent"]=true,["NotifyEvent"]=true},["done"]={["PointerDone"]=true}},"count"},
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
	{"length","int",{["given"]=allExcept(Given,{["CharGiv"]=true,["IntGiv"]=true})},{}},
	{"doubles","double",{["given"]={["DoublesGiv"]=true}},"length"},
	{"floats","float",{["given"]={["FloatsGiv"]=true}},"length"},
	{"ints","int",{["given"]={["IntsGiv"]=true}},"length"},
	{"chars","char",{["given"]={["CharsGiv"]=true}},"length"},
	{"key","char",{["given"]={["CharGiv"]=true}},{}},
	{"plane","int",{["given"]={["IntGiv"]=true}},{}},
}
Enums = {
	["Source"]=true,["Subconf"]=true,["Sculpt"]=true,["ClickMode"]=true,["MouseMode"]=true,["RollerMode"]=true,
	["TargetMode"]=true,["TopologyMode"]=true,["FixedMode"]=true,["Field"]=true,["Buffer"]=true,["Program"]=true,
	["Configure"]=true,["Function"]=true,["Event"]=true,["Equate"]=true,["Factor"]=true,["Change"]=true,["Given"]=true,
}
Structs = {
	["Format"]=true,["Feedback"]=true,["Update"]=true,["Render"]=true,["Command"]=true,["Data"]=true,
	["Term"]=true,["Sum"]=true,["Equ"]=true,["Sound"]=true,["State"]=true,["Query"]=true,
}
EnumOrder = {
	"Source","Subconf","Sculpt","ClickMode","MouseMode","RollerMode",
	"TargetMode","TopologyMode","FixedMode","Field","Buffer","Program",
	"Configure","Function","Event","Equate","Factor","Change","Given",
}
StructOrder = {
	"Format","Feedback","Update","Render","Command","Data","Term","Sum","Equ","Sound","State","Query",
}
function enumOf(str)
	if str == "Source" then return Source end
	if str == "Subconf" then return Subconf end
	if str == "Sculpt" then return Sculpt end
	if str == "ClickMode" then return ClickMode end
	if str == "MouseMode" then return MouseMode end
	if str == "RollerMode" then return RollerMode end
	if str == "TargetMode" then return TargetMode end
	if str == "TopologyMode" then return TopologyMode end
	if str == "FixedMode" then return FixedMode end
	if str == "Field" then return Field end
	if str == "Buffer" then return Buffer end
	if str == "Program" then return Program end
	if str == "Configure" then return Configure end
	if str == "Function" then return Function end
	if str == "Event" then return Event end
	if str == "Equate" then return Equate end
	if str == "Factor" then return Factor end
	if str == "Change" then return Change end
	if str == "Given" then return Given end
	return {}
end
function structOf(str)
	if str == "Format" then return Format end
	if str == "Feedback" then return Feedback end
	if str == "Update" then return Update end
	if str == "Render" then return Render end
	if str == "Command" then return Command end
	if str == "Data" then return Data end
	if str == "Term" then return Term end
	if str == "Sum" then return Sum end
	if str == "Equ" then return Equ end
	if str == "Sound" then return Sound end
	if str == "State" then return State end
	if str == "Query" then return Query end
	return {}
end
function printEnum(name,enum)
	print("enum "..name.." = {")
	for key,val in ipairs(enum) do
		print("    "..val..",")
	end
	print("};")
end
function stringAny(any)
	result = ""
	if type(any) == "nil" then result = "nil"
	elseif type(any) == "boolean" and not any then result = "false"
	elseif type(any) == "boolean" and any then result = "true"
	elseif type(any) == "number" then result = tostring(any)
	elseif type(any) == "string" then result = "\""..any.."\""
	elseif type(any) == "table" then
		result = "{"
		for key,val in pairs(any) do
			if result ~= "{" then result = result.."," end
			result = result.."["..stringAny(key).."]="..stringAny(val)
		end
		result = result.."}"
	else result = type(any) end
	return result
end
function stringIndent(depth)
	indent = ""; c = 0 while c < depth do c = c + 1; indent = indent.."    " end
	return indent
end
function printField(val,depth)
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
	print(stringIndent(depth)..decl.." "..ident..";")
end
function printStruct(name,struct)
	none = {[""]={}}
	all = none
	key = 1
	while struct[key] do
		tags = struct[key][3]
		print("name:"..stringAny(name).." field:"..stringAny(struct[key][1]).." key:"..stringAny(key).." tags:"..stringAny(tags).." all:"..stringAny(all))
		if tags ~= {} then all = unionDimSet(all,tags) end
		key = key + 1
	end
	length = 1
	mode = {"struct"} -- list of block types
	flds = {0} -- list of pointers to fields
	idxs = {{}} -- list of pointers to subblocks
	sizs = {0} -- list of sizes of idxs
	depth = 1
	univ = {all} -- stack of nested tag sets
	stack = {0} -- stack of pointers to open blocks
	key = 1
	while struct[key] do
		tags = struct[key][3]
		top = stack[depth]
		join = interDimSet(tags,univ[depth]) ~= none
		gain = differDimSet(tags,univ[depth]) ~= none
		loss = differDimSet(univ[depth],tags) ~= none
		print("name:"..stringAny(name).." key:"..stringAny(key).." field:"..stringAny(struct[key][1]).." tags:"..stringAny(tags).." univ:"..stringAny(univ[depth]).." depth:"..stringAny(depth).." top:"..stringAny(top).." join:"..stringAny(join).." gain:"..stringAny(gain).." loss:"..stringAny(loss))
		if mode[top] == "struct" and not gain and not loss then
			length = length + 1
			mode[length] = "field"
			flds[length] = key
			idxs[length] = {}
			sizs[length] = 0
			sizs[top] = sizs[top] + 1;
			idxs[top][sizs[top]] = length
			key = key + 1
		end
		if mode[top] == "struct" and not gain and loss then
			length = length + 1
			mode[length] = "union"
			flds[length] = 0
			idxs[length] = {}
			sizs[length] = 0
			depth = depth + 1
			univ[depth] = none
			stack[depth] = length
		end
		if mode[top] == "struct" and gain then
			depth = depth - 1
		end
		if mode[top] == "union" and not join then
			univ[depth] = unionDimSet(univ[depth],tags);
			length = length + 1
			mode[length] = "struct"
			flds[length] = 0
			idxs[length] = {}
			sizs[length] = 0
			depth = depth + 1
			univ[depth] = tags
			stack[depth] = length
		end
		if mode[top] == "union" and join then
			depth = depth - 1
		end
	end
	print("struct "..name.." {")
		indent = 1
		depth = 1
		stack = {sizs[1]}
		close = {false}
		index = 2
		while index <= length do
			if mode[index] == "struct" or mode[index] == "union" then
				depth = depth + 1
				stack[depth] = sizs[index]
				close[depth] = sizs[index] > 1
				if close[depth] then
					print(stringIndent(indent).."struct {")
					indent = indent + 1
				end
			end
			if mode[index] == "field" then
				print(stringIndent(indent)..stringAny(flds[index]))
				stack[depth] = stack[depth] - 1
				if stack[depth] == 0 and close[depth] then
					depth = depth - 1
					indent = indent - 1
					print(stringIndent(indent).."};")
				end
			end
			index = index + 1
		end
	print("};")
end
for key,val in ipairs(EnumOrder) do
	printEnum(val,enumOf(val))
end
for key,val in ipairs(StructOrder) do
	printStruct(val,structOf(val))
end
