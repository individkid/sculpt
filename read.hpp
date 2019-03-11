/*
*    read.hpp thread for reading and appending file
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

#ifndef READ_HPP
#define READ_HPP

#include "message.hpp"

class Window;
class Polytope;

class Read : public Thread
{
private:
	Window &data; // write mode and raw data to Window
	Polytope &read; // write -- to Polytope
	const char *name;
	int file;
	int pipe;
	int self;
	off_t fpos;
public:
	Message<Command> response; // get Command from Window
	Read(int i, Window &gl, Polytope &r, const char *n);
	virtual void init();
	virtual void call();
	virtual void wait();
	virtual void done();
};

#endif
