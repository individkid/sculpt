/*
*    parse.cpp function for converting string to Command
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

#include "parse.hpp"
extern "C" {
#include "callback.h"
}

const char *field[] = {"AllocField","WriteField","BindField","ReadField",0};
const char *buffer[] = {
	"Point0","Versor",
	"Point1","Normal","Coordinate","Weight","Color","Tag",
	"Point2",
	"Face1","Triple0","Triple1",
	"Pierce","Vector","Plane","Tagbits","Scalar",
	"Uniform","Query","Texture0","Texture1"};
const char *data[] = {"data","scalar","query"};
const char *name[] = {"getUniform","firstUniform","putUniform","checkQuery",0};
void (*function[])(struct Update *) = {getUniform,firstUniform,putUniform,checkQuery};
const char *program[] = {"Draw","Pierce","Sect0","Sect1","Side1","Side2"};

int Parse::get(const char *ptr, Update *&update)
{
	Update init = {0}; update = updates.get(); *update = init; int len = 0; int inc, i;
	if ((inc = number(ptr+len,update->file))) len += inc;
	else {updates.put(update); update = 0; return 0;}
	for (i = 0; i < Buffers; i++) {
		const char *pat = cleanup(concat(" ",buffer[i]));
		if ((inc = literal(ptr+len,pat))) {len += inc; update->buffer = (Buffer)i; break;}}
	if (i == Buffers) {updates.put(update); update = 0; return 0;}
	if ((inc = number(ptr+len,update->offset))) len += inc;
	else {updates.put(update); update = 0; return 0;}
	if ((inc = number(ptr+len,update->size))) len += inc;
	else {updates.put(update); update = 0; return 0;}
	int siz = (update->buffer == Texture0 || update->buffer == Texture1 ?
		update->width*update->height*3 + (4-(update->width*3)%4)*update->height :
		update->size);
	update->text = chars.get(siz);
	for (i = 0; i < siz; i++) {
		int val; if ((inc = number(ptr+len,val))) {len += inc; update->text[i] = val;}
		else {updates.put(update); update = 0; return 0;}}
	for (i = 0; name[i]; i++) {
		const char *pat = cleanup(concat(" ",name[i]));
		if ((inc = literal(ptr+len,pat))) {len += inc; update->function = function[i]; break;}}
	return len;
}

int Parse::get(const char *ptr, Render *&render)
{
	Render init = {0}; render = renders.get(); *render = init; int len = 0; int inc, i;
	if ((inc = number(ptr+len,render->file))) len += inc;
	else {renders.put(render); render = 0; return 0;}
	for (i = 0; i < Programs; i++) {
		const char *pat = cleanup(concat(" ",program[i]));
		if ((inc = literal(ptr+len,pat))) {len += inc; render->program = (Program)i; break;}}
	if (i == Programs) {renders.put(render); render = 0; return 0;}
	if ((inc = number(ptr+len,render->base))) len += inc;
	else {renders.put(render); render = 0; return 0;}
	if ((inc = number(ptr+len,render->count))) len += inc;
	else {renders.put(render); render = 0; return 0;}
	if ((inc = number(ptr+len,render->size))) len += inc;
	else {renders.put(render); render = 0; return 0;}
	return len;
}

int Parse::get(const char *ptr, int file, Command *&command)
{
	Command init = {0}; command = commands.get(); *command = init;
	int len = 0; int inc, i;
	if ((inc = literal(ptr+len," feedback"))) {len += inc; command->feedback = 1;}
	for (i = 0; i < Fields; i++) {
		const char *pat = cleanup(concat(" ",field[i]));
		Update *update = 0; int inval = 0;
		while ((inc = literal(ptr+len,pat))) {len += inc; Update *temp;
			inc = get(ptr+len,temp); if (inc) {len += inc;
			insert(update,temp);} else {inval = 1; break;}}
		while (update) {
            insert(command->update[(Field)i],update);
            remove(update,update);}
        if (inval) {put(command); command = 0; return 0;}}
    Render *render = 0; int inval = 0;
	while ((inc = literal(ptr+len," render"))) {len += inc; Render *temp;
		inc = get(ptr+len,temp); if (inc) {len += inc;
		insert(render,temp);} else {inval = 1; break;}}
	while (render) {
		insert(command->render,render);
		remove(render,render);}
    if (inval) {put(command); command = 0; return 0;}
	if ((inc = literal(ptr+len," redraw"))) {len += inc;
		inc = get(ptr+len,file,command->redraw); if (inc) len += inc;
		else {put(command); command = 0; return 0;}}
	if ((inc = literal(ptr+len," pierce"))) {len += inc;
		inc = get(ptr+len,file,command->pierce); if (inc) len += inc;
		else {put(command); command = 0; return 0;}}
	command->file = file;
	return len;
}

void Parse::script(const char *ptr, int file, enum Configure, Data *&data)
{
	// TODO
}

void Parse::text(const char *ptr, int file, enum Configure conf, Data *&data)
{
	int len = text(ptr);
	data = datas.get(); data->text = strncpy(setup(len+1),ptr,len);
	data->text[len] = 0; data->conf = conf; data->file = file;
}

void Parse::get(const char *ptr, int file, Sound *&sound)
{
	// TODO
}

void Parse::configure(const char *ptr, int file, enum Configure conf, Data *&data)
{
	data = datas.get();
	if (literal(ptr," Start")) data->subconf = StartSub;
	if (literal(ptr," Stop")) data->subconf = StopSub;
}

void Parse::get(const char *ptr, int file, enum Configure conf, Data *&data)
{
	data = datas.get(); data->matrix = floats.get(16);
	for (int i = 0; i < 16; i++) ptr += scalar(ptr, data->matrix[i]);
	data->conf = conf; data->file = file;
}

void Parse::get(const char *ptr, int file, Command *&command, Data *&window,
	Query *&query, Data *&polytope, Sound *&sound, Data *&system, Data *&data)
{
	int len, len1, len2, inc;
	command = 0; query = 0; sound = 0;
	window = polytope = system = data = 0;

	len = literal(ptr,"--additive"); len1 = literal(ptr+len,"click"); len2 = literal(ptr+len+len1,"");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = ClickUlpt; window->click = AdditiveMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"click"); len2 = literal(ptr+len+len1,"subractive");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = ClickUlpt; window->click = SubtractiveMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"click"); len2 = literal(ptr+len+len1,"refine");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = ClickUlpt; window->click = RefineMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"click"); len2 = literal(ptr+len+len1,"tweak");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = ClickUlpt; window->click = TweakMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"click"); len2 = literal(ptr+len+len1,"perform");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = ClickUlpt; window->click = PerformMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"click"); len2 = literal(ptr+len+len1,"transform");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = ClickUlpt; window->click = TransformMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"roller"); len2 = literal(ptr+len+len1,"cylinder");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = RollerUlpt; window->roller = CylinderMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"roller"); len2 = literal(ptr+len+len1,"clock");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = RollerUlpt; window->roller = ClockMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"roller"); len2 = literal(ptr+len+len1,"normal");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = RollerUlpt; window->roller = NormalMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"roller"); len2 = literal(ptr+len+len1,"parallel");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = RollerUlpt; window->roller = ParallelMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"roller"); len2 = literal(ptr+len+len1,"scale");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = RollerUlpt; window->roller = ScaleMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"mouse"); len2 = literal(ptr+len+len1,"rotate");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = MouseUlpt; window->mouse = RotateMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"mouse"); len2 = literal(ptr+len+len1,"tanget");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = MouseUlpt; window->mouse = TangentMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"mouse"); len2 = literal(ptr+len+len1,"translate");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = MouseUlpt; window->mouse = TranslateMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"target"); len2 = literal(ptr+len+len1,"session");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = TargetUlpt; window->target = SessionMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"target"); len2 = literal(ptr+len+len1,"polytope");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = TargetUlpt; window->target = PolytopeMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"target"); len2 = literal(ptr+len+len1,"facet");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = TargetUlpt; window->target = FacetMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"polytope"); len2 = literal(ptr+len+len1,"numeric");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = TopologyUlpt; window->topology = NumericMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"polytope"); len2 = literal(ptr+len+len1,"invariant");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = TopologyUlpt; window->topology = InvariantMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"polytope"); len2 = literal(ptr+len+len1,"symbolic");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = TopologyUlpt; window->topology = SymbolicMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"fixed"); len2 = literal(ptr+len+len1,"relative");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = FixedUlpt; window->fixed = RelativeMode; window->file = file; return;}

	len = literal(ptr,"--sculpt"); len1 = literal(ptr+len,"fixed"); len2 = literal(ptr+len+len1,"absolute");
	if (len && len1 && len2) {window = datas.get();
	window->conf = SculptConf; window->sculpt = FixedUlpt; window->fixed = AbsoluteMode; window->file = file; return;}

	len = literal(ptr,"--matrix"); if (len) {get(ptr+len,file,MatrixConf,window); return;}

	len = literal(ptr,"--global"); if (len) {get(ptr+len,file,GlobalConf,window); return;}

	len = literal(ptr,"--macro"); if (len) {script(ptr+len,file,MacroConf,window); return;}

	len = literal(ptr,"--sound"); if (len) {get(ptr+len,file,sound); return;}

	len = literal(ptr,"--metric"); if (len) {script(ptr+len,file,MetricConf,system); return;}

	len = literal(ptr,"--script"); if (len) {text(ptr+len,file,ScriptConf,data); return;}

	len = literal(ptr,"--invoke"); if (len) {text(ptr+len,file,InvokeConf,data); return;}

	len = literal(ptr,"--configure"); if (len) {configure(ptr+len,file,ConfigureConf,window); return;}

	len = literal(ptr,"--timewheel"); if (len) {configure(ptr+len,file,TimewheelConf,system); return;}

	len = literal(ptr,"--command"); if (len) {get(ptr+len,file,command); if (command) return;}

	len = literal(ptr,"--test"); if (len) {polytope = datas.get();
	inc = 0; while (ptr[len+inc]) if (ptr[len+inc] == '-' && ptr[len+inc+1] == '-') break; else inc++;
	polytope->text = prefix(ptr+len,inc+1); polytope->text[inc] = 0; len += inc;
	polytope->conf = TestConf; polytope->file = file; return;}
}

int Parse::number(const char *str, int &val)
{
	char *ptr;
	val = strtol(str,&ptr,0);
	return ptr-str;
}

int Parse::scalar(const char *str, float &val)
{
	char *ptr;
	val = strtof(str,&ptr);
	return ptr-str;
}

int Parse::literal(const char *str, const char *pat)
{
	const char *ptr = str;
	if (*pat && isspace(*pat)) {
	while (*pat && isspace(*pat)) pat++;
	while (*ptr && isspace(*ptr)) ptr++;}
	while (*pat && *ptr == *pat) {ptr++; pat++;}
	return (*pat ? 0 : ptr-str);
}

int Parse::text(const char *str)
{
	const char *ptr = str;
	while (literal(ptr,"--") == 0) ptr++;
	return ptr-str;
}
