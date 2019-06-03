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

#include "pools.hpp"

class Stream : public Pools
{
private:
	void get(int fd, Command *&command);
	void get(int fd, Update *&update);
	void get(int fd, Render *&render);
	void get(int fd, Manip *&manip);
	void get(int fd, Query *&query);
	void get(int fd, Data *&data);
	void get(int fd, char *&text);
	void put(int fd, char *text);
public:
	Stream(const char *file, int line) : Pools(file,line) {}
	Opcode get(int fd, Data *&data, Query *&query, Manip *&manip, Command *&command);
	void put(int fd, Opcode opcode, Command *command);
	void put(int fd, Opcode opcode, Manip *manip);
	void put(int fd, Opcode opcode, Query *query);
	void put(int fd, Opcode opcode, Data *data);
};

#endif
