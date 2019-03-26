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

#include "parse.hpp"

extern "C" void getUniform(struct Update *update);
extern "C" void firstUniform(struct Update *update);
extern "C" void putUniform(struct Update *update);
extern "C" void checkQuery(struct Update *update);

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
		const char *pat = cleanup(chars,concat(chars," ",buffer[i]));
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
		const char *pat = cleanup(chars,concat(chars," ",name[i]));
		if ((inc = literal(ptr+len,pat))) {len += inc; update->function = function[i]; break;}}
	return len;
}

int Parse::get(const char *ptr, Render *&render)
{
	Render init = {0}; render = renders.get(); *render = init; int len = 0; int inc, i;
	if ((inc = number(ptr+len,render->file))) len += inc;
	else {renders.put(render); render = 0; return 0;}
	for (i = 0; i < Programs; i++) {
		const char *pat = cleanup(chars,concat(chars," ",program[i]));
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
	Command init = {0}; command = commands.get(); *command = init; int len = 0; int inc, i;
	if ((inc = literal(ptr+len," feedback"))) {len += inc; command->feedback = 1;}
	for (i = 0; i < Fields; i++) {
		const char *pat = cleanup(chars,concat(chars," ",field[i]));
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
	Response *response = responses.get(); command->response = response;
	response->next = 0; response->file = file;
	return len;
}

void Parse::get(const char *ptr, int file, Command *&command, Sync *&sync, Mode *&mode,
	Data *&polytope, Data *&system, Data *&script)
{
	int len, inc; command = 0; sync = 0; mode = 0; polytope = 0;

	len = literal(ptr,"--additive"); if (len) {mode = modes.get();
	mode->mode = ClickType; mode->click = AdditiveMode; mode->file = file; return;}

	len = literal(ptr,"--subractive"); if (len) {mode = modes.get();
	mode->mode = ClickType; mode->click = SubtractiveMode; mode->file = file; return;}

	len = literal(ptr,"--refine"); if (len) {mode = modes.get();
	mode->mode = ClickType; mode->click = RefineMode; mode->file = file; return;}

	len = literal(ptr,"--tweak"); if (len) {mode = modes.get();
	mode->mode = ClickType; mode->click = TweakMode; mode->file = file; return;}

	len = literal(ptr,"--perform"); if (len) {mode = modes.get();
	mode->mode = ClickType; mode->click = PerformMode; mode->file = file; return;}

	len = literal(ptr,"--transform"); if (len) {mode = modes.get();
	mode->mode = ClickType; mode->click = TransformMode; mode->file = file; return;}

	len = literal(ptr,"--cylinder"); if (len) {mode = modes.get();
	mode->mode = RollerType; mode->roller = CylinderMode; mode->file = file; return;}

	len = literal(ptr,"--clock"); if (len) {mode = modes.get();
	mode->mode = RollerType; mode->roller = ClockMode; mode->file = file; return;}

	len = literal(ptr,"--normal"); if (len) {mode = modes.get();
	mode->mode = RollerType; mode->roller = NormalMode; mode->file = file; return;}

	len = literal(ptr,"--parallel"); if (len) {mode = modes.get();
	mode->mode = RollerType; mode->roller = ParallelMode; mode->file = file; return;}

	len = literal(ptr,"--scale"); if (len) {mode = modes.get();
	mode->mode = RollerType; mode->roller = ScaleMode; mode->file = file; return;}

	len = literal(ptr,"--rotate"); if (len) {mode = modes.get();
	mode->mode = MouseType; mode->mouse = RotateMode; mode->file = file; return;}

	len = literal(ptr,"--tanget"); if (len) {mode = modes.get();
	mode->mode = MouseType; mode->mouse = TangentMode; mode->file = file; return;}

	len = literal(ptr,"--translate"); if (len) {mode = modes.get();
	mode->mode = MouseType; mode->mouse = TranslateMode; mode->file = file; return;}

	len = literal(ptr,"--session"); if (len) {mode = modes.get();
	mode->mode = TargetType; mode->target = SessionMode; mode->file = file; return;}

	len = literal(ptr,"--polytope"); if (len) {mode = modes.get();
	mode->mode = TargetType; mode->target = PolytopeMode; mode->file = file; return;}

	len = literal(ptr,"--facet"); if (len) {mode = modes.get();
	mode->mode = TargetType; mode->target = FacetMode; mode->file = file; return;}

	len = literal(ptr,"--numeric"); if (len) {mode = modes.get();
	mode->mode = TopologyType; mode->topology = NumericMode; mode->file = file; return;}

	len = literal(ptr,"--invariant"); if (len) {mode = modes.get();
	mode->mode = TopologyType; mode->topology = InvariantMode; mode->file = file; return;}

	len = literal(ptr,"--symbolic"); if (len) {mode = modes.get();
	mode->mode = TopologyType; mode->topology = SymbolicMode; mode->file = file; return;}

	len = literal(ptr,"--relative"); if (len) {mode = modes.get();
	mode->mode = FixedType; mode->fixed = RelativeMode; mode->file = file; return;}

	len = literal(ptr,"--absolute"); if (len) {mode = modes.get();
	mode->mode = FixedType; mode->fixed = AbsoluteMode; mode->file = file; return;}

	len = literal(ptr,"--matrix"); if (len) {sync = syncs.get(); sync->matrix = floats.get(16);
	for (int i = 0; i < 16; i++) {inc = scalar(ptr+len, sync->matrix[i]); len += inc;}
	sync->target = PolytopeMode; sync->file = file; return;}

	len = literal(ptr,"--global"); if (len) {sync = syncs.get(); sync->matrix = floats.get(16);
	for (int i = 0; i < 16; i++) {inc = scalar(ptr+len, sync->matrix[i]); len += inc;}
	sync->target = SessionMode; sync->file = file; return;}

	len = literal(ptr,"--command"); if (len) {get(ptr+len,file,command); if (command) return;}

	len = literal(ptr,"--test"); if (len) {polytope = datas.get();
	inc = 0; while (ptr[len+inc]) if (ptr[len+inc] == '-' && ptr[len+inc+1] == '-') break; else inc++;
	polytope->text = prefix(chars,ptr+len,inc+1); polytope->text[inc] = 0; len += inc;
	polytope->conf = TestConf; polytope->file = file; return;}
}

void Parse::put(Sync *sync)
{
	if (sync->target == PolytopeMode || sync->target == SessionMode) floats.put(16,sync->matrix);
	syncs.put(sync);
}

void Parse::put(Mode *mode)
{
	modes.put(mode);
}

void Parse::put(Data *data)
{
	if (data->conf == TestConf) chars.put(strlen(data->text)+1,data->text);
	datas.put(data);
}

void Parse::put(Command *command)
{
	while (command) {
	for (int i = 0; i < Fields; i++) {
	Update *update = command->update[i];
	while (update) {updates.put(update); update = update->next;}}
	Render *render = command->render;
	while (render) {renders.put(render); render = render->next;}
	if (command->redraw) put(command->redraw);
	if (command->pierce) put(command->pierce);
	Response *response = command->response;
	while (response) {responses.put(response); response = response->next;}
	command = command->next;}
}
