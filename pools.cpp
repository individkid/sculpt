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
	command = command->next;}
}

void Pools::put(Manip *manip)
{
	// TODO
	manips.put(manip);
}

void Pools::put(Query *query)
{
	// TODO
	queries.put(query);
}

void Pools::put(Sound *sound)
{
	// TODO
	sounds.put(sound);
}

void Pools::put(Data *data)
{
	if (data->conf == MatrixConf || data->conf == GlobalConf) floats.put(16,data->matrix);
	if (data->conf == TestConf) chars.put(strlen(data->text)+1,data->text);
	datas.put(data);
}
