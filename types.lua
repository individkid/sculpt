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
invSource = {"ConfigureSrc","ModeSrc","MatrixSrc","GlobalSrc","PolytopeSrc","MacroSrc","HotkeySrc","Sources"}
Source = inverse(invSource)
invSubconf = {"StartSub","StopSub","Subconfs"}
Subconf = inverse(invSubconf)
invSculpt = {"ClickUlpt","MouseUlpt","RollerUlpt","TargetUlpt","TopologyUlpt","FixedUlpt","Sculpts"}
Sculpt = inverse(invSculpt)
invClickMode = {"AdditiveMode","SubtractiveMode","RefineMode","TweakMode","PerformMode","TransformMode","SuspendMode","PierceMode","ClickModes"}
ClickMode = inverse(invClickMode)
invMouseMode = {"RotateMode","TangentMode","TranslateMode","MouseModes"}
MouseMode = inverse(invMouseMode)
invRollerMode = {"CylinderMode","ClockMode","NormalMode","ParallelMode","ScaleMode","RollerModes"}
RollerMode = inverse(invRollerMode)
invTargetMode = {"SessionMode","PolytopeMode","FacetMode","TargetModes"}
TargetMode = inverse(invTargetMode)
invTopologyMode = {"NumericMode","InvariantMode","SymbolicMode","TopologyModes"}
TopologyMode = inverse(invTopologyMode)
invFixedMode = {"RelativeMode","AbsoluteMode","FixedModes"}
FixedMode = inverse(invFixedMode)
invField = {"AllocField","WriteField","BindField","ReadField","Fields"}
Field = inverse(invField)
invBuffer = {"Point0","Versor","Point1","Normal","Coordinate","Color","Weight","Tag","Point2","Face1","Triple0","Triple1","Texture0","Texture1","Uniform","Feedback","Inquery","Buffers"}
Buffer = inverse(invBuffer)
invProgram = {"Draw","Pierce","Sect0","Sect1","Side1","Side2","Programs"}
Program = inverse(invProgram)
Format = {
	{"cursor","float",nil,"array",{2}},
	{"affine","float",nil,"array",{16}},
	{"perplane","float",nil,"array",{16}},
	{"basis","float",nil,"array",{9,3}},
	{"cutoff","float",nil,"field",nil},
	{"slope","float",nil,"field",nil},
	{"aspect","float",nil,"field",nil},
	{"feather","float",nil,"array",{3}},
	{"arrow","float",nil,"array",{3}},
	{"enable","MYuint",nil,"field",nil},
	{"filler1","char",nil,"array",{0}},
	{"tagplane","MYuint",nil,"field",nil},
	{"filler2","char",nil,"array",{0}},
	{"taggraph","MYuint",nil,"field",nil},
};
Feedback = {
	{"pierce","float",function (s) return true end,"array",3},
	{"normal","float",nil,"array",3},
	{"tagbits","int",nil,"field",nil},
	{"plane","int",nil,"field",nil},
}

Update = {
	{"next","Update",function (s) return true end,"link",nil},
	{"file","int",nil,"field",nil},
	{"finish","int",nil,"field",nil},
	{"buffer","Buffer",nil,"field",nil},
	{"offset","int",function (s) return s.buffer ~= Buffer.Texture0 and s.buffer ~= Buffer.Texture1 end,"field",nil},
	{"width","int",function (s) return s.buffer == Buffer.Texture0 or s.buffer == Buffer.Texture1 end,"field",nil},
	{"size","int",function (s) return s.buffer ~= Buffer.Texture0 and s.buffer ~= Buffer.Texture1 end,"field",nil},
	{"height","int",function (s) return s.buffer == Buffer.Texture0 or s.buffer == Buffer.Texture1 end,"field",nil},
	{"format","Format",function (s) return s.buffer == Buffer.Update end,"struct",nil},
	{"feedback","Feedback",function (s) return s.buffer == Buffer.Feedback,"struct",nil},
	{"query","MYuint",function (s) return s.buffer == Buffer.Inquery end,"opaque",nil},
	{"data","void",function (s) return s.buffer < Buffer.Texture0 end,"scalars","size"},
	{"function","MYfunc",function (s) return true end,"opaque",nil},
}
