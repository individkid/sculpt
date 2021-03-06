/*
*    stream.hpp functions for converting pipe bytes to Command
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

#ifndef STREAM_HPP
#define STREAM_HPP

#include "message.hpp"

class Stream
{
private:
	void get(int fd, Command *&command);
	void get(int fd, Update *&update);
	void get(int fd, Render *&render);
	void get(int fd, Query *&query);
	void get(int fd, State *&state);
	void get(int fd, Data *&data);
public:
	Stream(const char *file, int line) {}
	Opcode get(int fd, Data *&data, State *&state,
		Query *&query, Command *&command);
	void put(int fd, Opcode opcode, Command *command);
	void put(int fd, Opcode opcode, Query *query);
	void put(int fd, Opcode opcode, State *state);
	void put(int fd, Opcode opcode, Data *data);
	void put(Command *command);
	void put(Query *query);
	void put(State *state);
	void put(Data *data);
};

#endif
