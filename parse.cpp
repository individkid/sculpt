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
#include <math.h>
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
	Response *response = responses.get(); command->response = response;
	response->next = 0; response->file = file;
	return len;
}

void Parse::get(const char *ptr, int file, enum Configure conf, Data *&data)
{
	data = datas.get(); data->matrix = floats.get(16);
	for (int i = 0; i < 16; i++) ptr += scalar(ptr, data->matrix[i]);
	data->conf = conf; data->file = file;
}

void Parse::get(const char *ptr, int file, Command *&command,
	Data *&window, Data *&polytope, Data *&system, Data *&script)
{
	int len, inc; command = 0; window = polytope = system = script = 0;

	len = literal(ptr,"--additive"); if (len) {window = datas.get();
	window->conf = AdditiveConf; window->file = file; return;}

	len = literal(ptr,"--subractive"); if (len) {window = datas.get();
	window->conf = SubtractiveConf; window->file = file; return;}

	len = literal(ptr,"--refine"); if (len) {window = datas.get();
	window->conf = RefineConf; window->file = file; return;}

	len = literal(ptr,"--tweak"); if (len) {window = datas.get();
	window->conf = TweakConf; window->file = file; return;}

	len = literal(ptr,"--perform"); if (len) {window = datas.get();
	window->conf = PerformConf; window->file = file; return;}

	len = literal(ptr,"--transform"); if (len) {window = datas.get();
	window->conf = TransformConf; window->file = file; return;}

	len = literal(ptr,"--cylinder"); if (len) {window = datas.get();
	window->conf = CylinderConf; window->file = file; return;}

	len = literal(ptr,"--clock"); if (len) {window = datas.get();
	window->conf = ClockConf; window->file = file; return;}

	len = literal(ptr,"--normal"); if (len) {window = datas.get();
	window->conf = NormalConf; window->file = file; return;}

	len = literal(ptr,"--parallel"); if (len) {window = datas.get();
	window->conf = ParallelConf; window->file = file; return;}

	len = literal(ptr,"--scale"); if (len) {window = datas.get();
	window->conf = ScaleConf; window->file = file; return;}

	len = literal(ptr,"--rotate"); if (len) {window = datas.get();
	window->conf = RotateConf; window->file = file; return;}

	len = literal(ptr,"--tanget"); if (len) {window = datas.get();
	window->conf = TangentConf; window->file = file; return;}

	len = literal(ptr,"--translate"); if (len) {window = datas.get();
	window->conf = TranslateConf; window->file = file; return;}

	len = literal(ptr,"--session"); if (len) {window = datas.get();
	window->conf = SessionConf; window->file = file; return;}

	len = literal(ptr,"--polytope"); if (len) {window = datas.get();
	window->conf = PolytopeConf; window->file = file; return;}

	len = literal(ptr,"--facet"); if (len) {window = datas.get();
	window->conf = FacetConf; window->file = file; return;}

	len = literal(ptr,"--numeric"); if (len) {window = datas.get();
	window->conf = NumericConf; window->file = file; return;}

	len = literal(ptr,"--invariant"); if (len) {window = datas.get();
	window->conf = InvariantConf; window->file = file; return;}

	len = literal(ptr,"--symbolic"); if (len) {window = datas.get();
	window->conf = SymbolicConf; window->file = file; return;}

	len = literal(ptr,"--relative"); if (len) {window = datas.get();
	window->conf = RelativeConf; window->file = file; return;}

	len = literal(ptr,"--absolute"); if (len) {window = datas.get();
	window->conf = AbsoluteConf; window->file = file; return;}

	len = literal(ptr,"--matrix"); if (len) {get(ptr+len,file,MatrixConf,window); return;}

	len = literal(ptr,"--global"); if (len) {get(ptr+len,file,GlobalConf,window); return;}

	len = literal(ptr,"--command"); if (len) {get(ptr+len,file,command); if (command) return;}

	len = literal(ptr,"--test"); if (len) {polytope = datas.get();
	inc = 0; while (ptr[len+inc]) if (ptr[len+inc] == '-' && ptr[len+inc+1] == '-') break; else inc++;
	polytope->text = prefix(ptr+len,inc+1); polytope->text[inc] = 0; len += inc;
	polytope->conf = TestConf; polytope->file = file; return;}
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

void Parse::put(Data *data)
{
	if (data->conf == MatrixConf || data->conf == GlobalConf) floats.put(16,data->matrix);
	if (data->conf == TestConf) cleanup(data->text);
	datas.put(data);
}

char *Parse::get(const Data *data)
{
	switch (data->conf) {
	case (AdditiveConf): return setup("--additive");
	case (SubtractiveConf): return setup("--subtractive");
	case (RefineConf): return setup("--refine");
	case (TweakConf): return setup("--tweak");
	case (PerformConf): return setup("--perform");
	case (TransformConf): return setup("--transform");
	case (CylinderConf): return setup("--cylinder");
	case (ClockConf): return setup("--clock");
	case (NormalConf): return setup("--normal");
	case (ParallelConf): return setup("--parallel");
	case (ScaleConf): return setup("--scale");
	case (RotateConf): return setup("--rotate");
	case (TangentConf): return setup("--tangent");
	case (TranslateConf): return setup("--translate");
	case (SessionConf): return setup("--session");
	case (PolytopeConf): return setup("--polytope");
	case (FacetConf): return setup("--facet");
	case (NumericConf): return setup("--numeric");
	case (InvariantConf): return setup("--invariant");
	case (SymbolicConf): return setup("--symbolic");
	case (RelativeConf): return setup("--relative");
	case (AbsoluteConf): return setup("--absolute");
	case (IncludeConf): {const char *str = data->text; return concat(setup("--include "),str);}
	case (ExcludeConf): return setup("--exclude");
	case (PlaneConf): {char *str = setup("--plane #");
		str = concat(str,string(data->plane));
		str = concat(str,string(data->versor));
		for (int i = 0; i < 3; i++) {
			str = concat(str," "); str = concat(str,string(data->vector[i]));}
		return str;}
	case (MatrixConf): {char *str = setup("--matrix ");
		str = concat(str,string(data->seqnum));
		for (int i = 0; i < 16; i++) {
			str = concat(str," "); str = concat(str,string(data->vector[i]));}
		int len = strlen(str);
		while (len < MMAP) {str = concat(str," "); len++;}
		return str;}
	case (GlobalConf): {char *str = setup("--global ");
		str = concat(str,string(data->seqnum));
		for (int i = 0; i < 16; i++) {
			str = concat(str," "); str = concat(str,string(data->vector[i]));}
		int len = strlen(str);
		while (len < MMAP) {str = concat(str," "); len++;}
		return str;}
	case (SpaceConf): {char *str = setup("--space");
		for (int i = 0; i < data->boundaries; i++) {
			str = concat(str," #"); str = concat(str,string(data->planes[i]));
			for (int j = 0; j < data->regions; j++) {
				str = concat(str," "); str = concat(str,string(data->sides[i][j]));}}
		return str;}
	case (RegionConf): {char *str = setup("--region ");
		str = concat(str,string(data->side));
		str = concat(str," "); str = concat(str,string(data->insides));
		for (int i = 0; i < data->insides; i++) {
			str = concat(str," #"); str = concat(str,string(data->inside[i]));}
		str = concat(str," "); str = concat(str,string(data->outsides));
		for (int i = 0; i < data->outsides; i++) {
			str = concat(str, " #"); str = concat(str,string(data->outside[i]));}
		return str;}
	case (InflateConf): return setup("--inflate");
	case (PictureConf): {const char *str = data->text; return concat(setup("--picture "),str);}
	case (SoundConf): /*TODO*/ break;
	case (MetricConf): /*TODO*/ break;
	case (ScriptConf): /*TODO*/ break;
	case (CommandConf): /*TODO*/ break;
	case (ConfigureConf): /*TODO*/ break;
	case (TestConf): {const char *str = data->text; return concat(setup("--test "),str);}
	default: error("invalid conf",data->conf,__FILE__,__LINE__);}
	return 0;
}

char *Parse::string(int val)
{
	int len = 0; int tmp = val; int rem = 0; char *str;
	if (val < 0) tmp = -tmp;
	while (tmp) {rem = tmp % 10; tmp = tmp / 10; len++;}
	if (val == 0) len++;
	if (val < 0) {len++; tmp = -tmp;}
	str = chars.get(len+1); len = 0; tmp = val;
	if (val < 0) tmp = -tmp;
	while (tmp) {rem = tmp % 10; tmp = tmp / 10; str[len++] = '0' + rem;}
	if (val == 0) str[len++] = '0';
	if (val < 0) {str[len++] = '-'; tmp = -tmp;}
	for (int i = 0; i < len/2; i++) {char chr = str[i]; str[i] = str[len-1-i]; str[len-1-i] = chr;}
	str[len] = 0; return str;
}

char *Parse::string(float val)
{
	float tmp, mag; int exp; int len = 2; int rem = 0; char *str;
	tmp = frexp(val,&exp);
	if (val > 0) mag = tmp; else {mag = -tmp; len++;}
	if (mag <= 1.0/INVALID) len++;
	while (mag > 1.0/INVALID) {mag = mag * 10.0; rem = (int)mag; mag = mag - (float)rem; len++;}
	str = chars.get(len); len = 2;
	if (val > 0) mag = tmp; else {mag = -tmp; str[len++] = '-';}
	str[len++] = '0'; str[len++] = '.';
	if (mag <= 1.0/INVALID) str[len++] = '0';
	while (mag > 1.0/INVALID) {mag = mag * 10.0; rem = (int)mag; mag = mag - (float)rem; str[len++] = '0'+rem;}
	return concat(string(exp),concat("E",str));
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
