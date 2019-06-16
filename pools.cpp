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
	put(&sound->equat); put(&sound->delay); put(&sound->sched);
	put(&sound->left); put(&sound->right);
	Sound *temp = sound; sound = sound->next; sounds.put(temp);}
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
	case (MacroConf): chars.put(strlen(data->script)+1,data->script); break;
	case (HotkeyConf): chars.put(strlen(data->script)+1,data->script); break;
	case (MetricConf): ints.put(data->count,data->ident);
	doubles.put(data->count,data->value);
	chars.put(strlen(data->metric)+1,data->metric); break;
	case (NotifyConf):
	chars.put(strlen(data->metric)+1,data->metric); break;
	case (ConfigureConf): break;
	case (TimewheelConf): break;
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
