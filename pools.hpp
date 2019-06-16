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
	Pool<Manip> manips;
	Pool<Query> queries;
	Pool<Sound> sounds;
	Pool<Data> datas;
	Power<Term> terms;
	Power<double*> pointers;
	Power<double> doubles;
	Power<float> floats;
	Power<char> chars;
	Power<int> ints;
public:
	Pools(const char *file, int line) :
		commands(file,line), updates(file,line), renders(file,line),
		manips(file,line), queries(file,line), sounds(file,line), datas(file,line),
		terms(file,line), pointers(file,line), doubles(file,line),
		floats(file,line), chars(file,line), ints(file,line) {}
	void put(Command *command);
	void put(Manip *manip);
	void put(Query *query);
	void put(Sound *sound);
	void put(Data *data);
private:
	void put(Term *term);
	void put(Sum *sum);
	void put(Equ *equ);
};

#endif
