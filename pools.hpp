/*
*    pools.hpp pools for data structures
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

#ifndef POOLS_HPP
#define POOLS_HPP

#include "message.hpp"

class Pools
{
protected:
	Pool<Command> commands;
	Pool<Update> updates;
	Pool<Render> renders;
	Pool<Data> datas;
	Power<float> floats;
	Power<char> chars;
public:
	Pools(const char *file, int line) :
		commands(file,line), updates(file,line), renders(file,line),
		datas(file,line), floats(file,line), chars(file,line) {}
	void put(Command *command);
	void put(Sound *sound);
	void put(Data *data);
};

#endif