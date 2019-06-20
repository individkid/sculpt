/*
*    pools.cpp pools for data structures
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

#include "pools.hpp"

int Holes::get()
{
	if (range.begin() == range.end()) {
		Range zero; zero.base = 0; zero.size = 1;
		range.insert(zero);
		return 0;}
	std::set<Range,Range>::iterator iter = range.begin();
	Range elem = *iter;
	int val;
	if (elem.base == 0) {
		val = elem.size++;
		std::set<Range,Range>::iterator after = iter;
		if (++after != range.end()) {
			Range post = *after;
			if (elem.base+elem.size == post.base) {
				elem.size += post.size;
				range.erase(after);}}}
	else {
		val = elem.base--;
		std::set<Range,Range>::iterator before = iter;
		if (--before != range.end()) {
			Range prev = *before;
			if (prev.base+prev.size == elem.base) {
				elem.base = prev.base;
				range.erase(before);}}}
	range.erase(iter);
	range.insert(elem);
	return val;
}

int Holes::get(int num)
{
	Unique key; key.ident = NumberIdent; key.number = num;
	if (unique.find(key) == unique.end()) unique[key] = get();
	return unique[key];
}

int Holes::get(const char *nam)
{
	Unique key; key.ident = NameIdent; key.name = nam;
	if (unique.find(key) == unique.end()) unique[key] = get();
	return unique[key];
}

void Pools::put(Command *command)
{
	while (command) {
	for (int i = 0; i < Fields; i++) {
	Update *update = command->update[i];
	while (update) {updates.put(update); update = update->next;}}
	Render *render = command->render;
	while (render) {renders.put(render); render = render->next;}
	if (command->redraw) put(command->redraw);
	if (command->pierce) put(command->pierce);
	Command *temp = command; command = command->next; commands.put(temp);}
}

void Pools::put(Manip *manip)
{
	while (manip) {
	switch (manip->click) {
	case (AdditiveMode): break;
	case (SubtractiveMode): break;
	case (RefineMode): floats.put(3,manip->vector); break;
	case (TweakMode): floats.put(3,manip->vector); break;
	case (PerformMode): break;
	case (TransformMode): floats.put(16,manip->vector); break;
	default: error("invalid click",manip->click,__FILE__,__LINE__);}
	Manip *temp = manip; manip = manip->next; manips.put(temp);}
}

void Pools::put(Query *query)
{
	while (query) {
	if (query->text) chars.put(strlen(query->text)+1,query->text);
	Query *temp = query; query = query->next; queries.put(temp);}
}

void Pools::put(Sound *sound)
{
	while (sound) {
	switch (sound->sync) {
	case (StartSync): break;
	case (StopSync): break;
	case (ScriptSync):
	doubles.put(sound->params,sound->values);
	if (sound->metric) {sound->metric->count--;
	if (sound->metric->count == 0) {
	chars.put(strlen(sound->metric->ptr)+1,sound->metric->ptr);
	smarts.put(sound->metric);}}
	break;
	case (MetricSync):
	ints.put(sound->count,sound->ids); pointers.put(sound->count,sound->ptrs);
	chars.put(strlen(sound->script)+1,sound->script);
	if (sound->smart) {sound->smart->count--;
	if (sound->smart->count == 0) {
	chars.put(strlen(sound->smart->ptr)+1,sound->smart->ptr);
	smarts.put(sound->smart);}}
	break;
	case (UpdateSync): break;
	case (SoundSync):
	for (int i = 0; i < Equates; i++) put(&sound->equ[i]);
	break;
	default: error("invalid sync",sound->sync,__FILE__,__LINE__);}
	Sound *temp = sound; sound = sound->next; sounds.put(temp);}
}

void Pools::put(State *state)
{
	// TODO
	states.put(state);
}

void Pools::put(Data *data)
{
	while (data) {
	switch (data->conf) {
	case (SculptConf): break;
	case (GlobalConf): floats.put(16,data->matrix); break;
	case (MatrixConf): floats.put(16,data->matrix); break;
	case (PlaneConf): floats.put(3,data->matrix); break;
	case (PictureConf): chars.put(strlen(data->filename)+1,data->filename); break;
	case (SpaceConf): ints.put(data->boundaries,data->planes);
	ints.put(data->boundaries*data->regions,data->sides); break;
	case (RegionConf): ints.put(data->insides,data->inside); 
	ints.put(data->outsides,data->outside); break;
	case (InflateConf): break;
	case (PolytopeConf): /*TODO*/ break;
	case (IncludeConf): break;
	case (ScriptConf): chars.put(strlen(data->script)+1,data->script); break;
	case (PauseConf): chars.put(strlen(data->script)+1,data->script); break;
	case (MacroConf): chars.put(strlen(data->script)+1,data->script); break;
	case (HotkeyConf): chars.put(strlen(data->script)+1,data->script); break;
	case (ConfigureConf): break;
	default: error("invalid conf",data->conf,__FILE__,__LINE__);}
	Data *temp = data; data = data->next; datas.put(data);}
}

void Pools::put(Term *term)
{
	switch (term->factor) {
	case (ConstFactor): break;
	case (VaryFactor): ints.put(1,term->id); pointers.put(1,term->ptr); break;
	case (SquareFactor): ints.put(2,term->id); pointers.put(2,term->ptr); break;
	case (CompFactor): ints.put(3,term->id); pointers.put(3,term->ptr); break;
	default: error("invalid factor",term->factor,__FILE__,__LINE__);}
}

void Pools::put(Sum *sum)
{
	for (int i = 0; i < sum->count; i++) put(&sum->term[i]);
	if (sum->count > 0) terms.put(sum->count,sum->term);
}

void Pools::put(Equ *equ)
{
	put(&equ->numer);
	put(&equ->denom);
}
