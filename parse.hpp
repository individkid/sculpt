/*
*    parse.hpp function for converting string to Command
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

#include "types.h"
#include "message.hpp"

class Parse
{
public:
	Pool<Command> commands;
	Pool<Update> updates;
	Pool<Render> renders;
	Pool<Response> responses;
	Pool<Sync> syncs;
	Pool<Mode> modes;
	Pool<Data> datas;
	Power<char> chars;
	void get(const char *&ptr, Update *&update);
	void get(const char *&ptr, Render *&render);
	void get(const char *&ptr, int file, Command *&command);
	void get(const char *ptr, int file, Command *&command, Sync *&sync, Mode *&mode, Sync *&polytope);
	void put(Command *command);
	void put(Sync *sync);
	void put(Mode *mode);
	void put(Data *data);
};
