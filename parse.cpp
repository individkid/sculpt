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
	"Uniform","Query","Texture0","Texture1",0};
const char *data[] = {"data","scalar","query",0};
const char *name[] = {"getUniform","firstUniform","putUniform","checkQuery",0};
void (*function[])(struct Update *) = {getUniform,firstUniform,putUniform,checkQuery,0};
const char *program[] = {"Draw","Pierce","Sect0","Sect1","Side1","Side2",0};
const char *subconf[] = {0};

int Parse::get(const char *&ptr, Update *&update)
{
	Update init = {0}; update = updates.get(); *update = init; int i;
	if (number(ptr,update->file)) ;
	else {updates.put(update); update = 0; return 0;}
	for (i = 0; i < Buffers; i++) {
		if (literal(ptr,buffer[i])) {update->buffer = (Buffer)i; break;}}
	if (i == Buffers) {updates.put(update); update = 0; return 0;}
	if (number(ptr,update->offset)) ;
	else {updates.put(update); update = 0; return 0;}
	if (number(ptr,update->size)) ;
	else {updates.put(update); update = 0; return 0;}
	int siz = (update->buffer == Texture0 || update->buffer == Texture1 ?
		update->width*update->height*3 + (4-(update->width*3)%4)*update->height :
		update->size);
	update->text = chars.get(siz);
	for (i = 0; i < siz; i++) {
		int val; if (number(ptr,val)) {update->text[i] = val;}
		else {updates.put(update); update = 0; return 0;}}
	for (i = 0; name[i]; i++) {
		if (literal(ptr,name[i])) {update->function = function[i]; break;}}
	return 1;
}

int Parse::get(const char *&ptr, Render *&render)
{
	Render init = {0}; render = renders.get(); *render = init; int i;
	if (number(ptr,render->file)) ;
	else {renders.put(render); render = 0; return 0;}
	for (i = 0; i < Programs; i++) {
		if (literal(ptr,program[i])) {render->program = (Program)i; break;}}
	if (i == Programs) {renders.put(render); render = 0; return 0;}
	if (number(ptr,render->base)) ;
	else {renders.put(render); render = 0; return 0;}
	if (number(ptr,render->count)) ;
	else {renders.put(render); render = 0; return 0;}
	if (number(ptr,render->size)) ;
	else {renders.put(render); render = 0; return 0;}
	return 1;
}

int Parse::get(const char *&ptr, int file, Command *&command)
{
	Command init = {0}; command = commands.get(); *command = init;
	int i;
	if (literal(ptr,"feedback")) {command->feedback = 1;}
	for (i = 0; i < Fields; i++) {
		Update *update = 0; int inval = 0;
		while (literal(ptr,field[i])) {Update *temp;
			if (get(ptr,temp)) {insert(update,temp);} else {inval = 1; break;}}
		while (update) {
            insert(command->update[(Field)i],update);
            remove(update,update);}
        if (inval) {put(command); command = 0; return 0;}}
    Render *render = 0; int inval = 0;
	while (literal(ptr,"render")) {Render *temp;
		if (get(ptr,temp)) {insert(render,temp);} else {inval = 1; break;}}
	while (render) {
		insert(command->render,render);
		remove(render,render);}
    if (inval) {put(command); command = 0; return 0;}
	if (literal(ptr,"redraw")) {
		if (get(ptr,file,command->redraw)) ;
		else {put(command); command = 0; return 0;}}
	if (literal(ptr,"pierce")) {
		if (get(ptr,file,command->pierce)) ;
		else {put(command); command = 0; return 0;}}
	command->file = file;
	return 1;
}

int Parse::get(const char *&ptr, int file, Manip *&manip)
{
	// TODO
	return 0;
}

int Parse::get(const char *&ptr, int file, Sound *&sound)
{
	// TODO
	return 0;
}

int Parse::get(const char *&ptr, int file, Query *&query)
{
	// TODO
	return 0;
}

#define CONF_SCULPT(ULPTS,MODES,ULPT,MODE) \
	if (literal(ptr,"--sculpt") && alloc(window) && literal(ptr,#ULPTS) && \
	literal(ptr,#MODES)) {window->file = file; window->conf = SculptConf; \
	window->sculpt = ULPT##Ulpt; window->ULPTS = MODE##Mode; return 1;} \
	else {deloc(window); ptr = sav;}

int Parse::get(const char *&ptr, int file, Command *&command, Data *&window, Manip *&manip,
	Query *&query, Data *&polytope, Sound *&sound, Data *&system, Data *&script)
{
	command = 0; manip = 0; query = 0; sound = 0;
	window = polytope = system = script = 0;
	const char *sav = ptr;

	CONF_SCULPT(click,additive,Click,Additive);
	CONF_SCULPT(click,subractive,Click,Subtractive);
	CONF_SCULPT(click,refine,Click,Refine);
	CONF_SCULPT(click,tweak,Click,Tweak);
	CONF_SCULPT(click,perform,Click,Perform);
	CONF_SCULPT(click,transform,Click,Transform);
	CONF_SCULPT(roller,cylinder,Roller,Cylinder);
	CONF_SCULPT(roller,clock,Roller,Clock);
	CONF_SCULPT(roller,normal,Roller,Normal);
	CONF_SCULPT(roller,parallel,Roller,Parallel);
	CONF_SCULPT(roller,scale,Roller,Scale);
	CONF_SCULPT(mouse,rotate,Mouse,Rotate);
	CONF_SCULPT(mouse,tanget,Mouse,Tangent);
	CONF_SCULPT(mouse,translate,Mouse,Translate);
	CONF_SCULPT(target,session,Target,Session)
	CONF_SCULPT(target,polytope,Target,Polytope)
	CONF_SCULPT(target,facet,Target,Facet)
	CONF_SCULPT(topology,numeric,Topology,Numeric)
	CONF_SCULPT(topology,invariant,Topology,Invariant)
	CONF_SCULPT(topology,symbolic,Topology,Symbolic)
	CONF_SCULPT(fixed,relative,Fixed,Relative)
	CONF_SCULPT(fixed,absolute,Fixed,Absolute)

	if (literal(ptr,"--global") && alloc(window) && scalars(ptr,16,window->matrix)) {
	window->file = file; window->conf = GlobalConf; return 1;}
	else {deloc(16,window->matrix); deloc(window); ptr = sav;}
	if (literal(ptr,"--matrix") && alloc(window) && scalars(ptr,16,window->matrix)) {
	window->file = file; window->conf = MatrixConf; return 1;}
	else {deloc(16,window->matrix); deloc(window); ptr = sav;}
	if (literal(ptr,"--plane") && alloc(polytope) && identifier(ptr,polytope->plane) &&
	number(ptr,polytope->versor) && scalars(ptr,3,polytope->vector)) {
	polytope->file = file; polytope->conf = PlaneConf; return 1;}
	else {deloc(3,polytope->vector); deloc(polytope); ptr = sav;}
	if (literal(ptr,"--picture") && alloc(polytope) && identifier(ptr,polytope->plane) &&
	word(ptr,polytope->filename)) {
	polytope->file = file; polytope->conf = PictureConf; return 1;}
	else {deloc(polytope->filename); deloc(polytope); ptr = sav;}
	if (literal(ptr,"--space") && number(ptr,polytope->boundaries) && number(ptr,polytope->regions) &&
	identifiers(ptr,polytope->boundaries,polytope->planes) &&
	numbers(ptr,polytope->boundaries*polytope->regions,polytope->sides)) {
	polytope->file = file; polytope->conf = SpaceConf; return 1;}
	else {deloc(polytope->boundaries,polytope->planes);
	deloc(polytope->boundaries*polytope->regions,polytope->sides); deloc(polytope); ptr = sav;}
	if (literal(ptr,"--region") && number(ptr,polytope->side) &&
	number(ptr,polytope->insides) && number(ptr,polytope->outsides) &&
	identifiers(ptr,polytope->insides,polytope->inside) &&
	identifiers(ptr,polytope->outsides,polytope->outside)) {
	polytope->file = file; polytope->conf = RegionConf; return 1;}
	else {deloc(polytope->insides,polytope->inside); deloc(polytope->outsides,polytope->outside);
	deloc(polytope); ptr = sav;}
	if (literal(ptr,"--inflate") && alloc(polytope)) {
	polytope->file = file; polytope->conf = InflateConf; return 1;}
	else {deloc(polytope); ptr = sav;}
	if (literal(ptr,"--polytope") && alloc(polytope) && 0/*TODO*/) {
	polytope->file = file; polytope->conf = PolytopeConf; return 1;}
	else {/*TODO*/ deloc(polytope); ptr = sav;}
	if (literal(ptr,"--include") && alloc(polytope) && word(ptr,polytope->filename)) {
	polytope->conf = IncludeConf; return 1;}
	else {deloc(polytope); ptr = sav;}
	if (literal(ptr,"--command") && get(ptr,file,command)) return 1;
	else {ptr = sav;}
	if (literal(ptr,"--sound") && get(ptr,file,sound)) return 1;
	else {ptr = sav;}
	if (literal(ptr,"--script") && alloc(script) && text(ptr,script->script)) {
	script->file = file; script->conf = ScriptConf; return 1;}
	else {deloc(script->script); deloc(script); ptr = sav;}
	if (literal(ptr,"--pause") && alloc(script) && text(ptr,script->script)) {
	script->file = file; script->conf = PauseConf; return 1;}
	else {deloc(script->script); deloc(script); ptr = sav;}
	if (literal(ptr,"--macro") && alloc(window) && text(ptr,window->script)) {
	window->file = file; window->conf = MacroConf; return 1;}
	else {deloc(window->script); deloc(window); ptr = sav;}
	if (literal(ptr,"--hotkey") && alloc(window) && text(ptr,window->script)) {
	window->file = file; window->conf = HotkeyConf; return 1;}
	else {deloc(window->script); deloc(window); ptr = sav;}
	if (literal(ptr,"--metric") && alloc(system) && scalar(ptr,system->delay) &&
	number(ptr,system->count) && identifiers(ptr,system->count,system->ident) && text(ptr,system->metric)) {
	system->file = file; system->conf = MetricConf; return 1;}
	else {deloc(system->count,system->ident); deloc(system->metric); deloc(system); ptr = sav;}
	if (literal(ptr,"--query") && get(ptr,file,query)) return 1;
	else {ptr = sav;}
	if (literal(ptr,"--configure") && alloc(window) &&
	subconf(ptr,window->subconf) && scalar(ptr,window->setting)) {
	window->file = file; window->conf = ConfigureConf; return 1;}
	else {deloc(window); ptr = sav;}
	if (literal(ptr,"--timewheel") && alloc(system) &&
	subconf(ptr,window->subconf) && scalar(ptr,system->setting)) {
	system->file = file; system->conf = TimewheelConf; return 1;}
	else {deloc(system); ptr = sav;}

	if (ptr[0] == '-' && ptr[1] == '-') ptr += 2;
	while (ptr[0] != 0 && !(ptr[0] == '-' && ptr[1] == '-')) ptr++;
	if (ptr[0] == 0) ptr = sav;
	return 0;
}

int Parse::identifier(const char *&str, int &val)
{
	// TODO
	return 0;
}

int Parse::number(const char *&str, int &val)
{
	char *ptr;
	val = strtol(str,&ptr,0);
	if (ptr == str) return 0;
	str = ptr;
	return 1;
}

int Parse::scalar(const char *&str, float &val)
{
	char *ptr;
	val = strtof(str,&ptr);
	if (ptr == str) return 0;
	str = ptr;
	return 1;
}

int Parse::literal(const char *&str, const char *pat)
{
	const char *ptr = str;
	if (*pat && isspace(*pat)) {
	while (*pat && isspace(*pat)) pat++;
	while (*ptr && isspace(*ptr)) ptr++;}
	while (*pat && *ptr == *pat) {ptr++; pat++;}
	if (*pat) return 0;
	str = ptr;
	return 1;
}

int Parse::text(const char *&str, char *&val)
{
	const char *ptr = str;
	while (literal(ptr,"--") == 0) ptr++;
	if (ptr == str) return 0;
	int len = ptr-str;
	val = chars.get(len+1);
	strncpy(val,str,len);
	val[len] = 0;
	str = ptr;
	return 1;
}

int Parse::word(const char *&str, char *&val)
{
	const char *ptr = str;
	while (literal(ptr,"--") == 0 && !isspace(*ptr)) ptr++;
	if (ptr == str) return 0;
	int len = ptr-str;
	val = chars.get(len+1);
	strncpy(val,str,len);
	val[len] = 0;
	str = ptr;
	return 1;
}

int Parse::identifiers(const char *&str, int siz, int *&val)
{
	val = ints.get(siz);
	for (int i = 0; i < siz; i++) if (!identifier(str,val[i])) return 0;
	return 0;
}

int Parse::numbers(const char *&str, int siz, int *&val)
{
	val = ints.get(siz);
	for (int i = 0; i < siz; i++) if (!number(str,val[i])) return 0;
	return 1;
}

int Parse::scalars(const char *&str, int siz, float *&val)
{
	val = floats.get(siz);
	for (int i = 0; i < siz; i++) if (!scalar(str,val[i])) return 0;
	return 1;
}

int Parse::subconf(const char *&str, enum Subconf &val)
{
	for (int i = 0; ::subconf[i]; i++) if (literal(str,::subconf[i])) return 0;
	return 1;
}

int Parse::alloc(Data *&val)
{
	Data init = {0};
	val = datas.get(); *val = init;
	return 1;
}

int Parse::deloc(int siz, int *&val)
{
	if (siz == 0 || val == 0) return 0;
	ints.put(siz,val);
	val = 0;
	return 1;
}

int Parse::deloc(int siz, float *&val)
{
	if (siz == 0 || val == 0) return 0;
	floats.put(siz,val);
	val = 0;
	return 1;
}

int Parse::deloc(char *&val)
{
	if (val == 0) return 0;
	chars.put(strlen(val)+1,val);
	val = 0;
	return 1;
}

int Parse::deloc(Data *&val)
{
	if (val == 0) return 0;
	datas.put(val);
	val = 0;
	return 1;
}
