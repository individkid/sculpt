/*
*    write.hpp thread for writing to pipe
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

#ifndef WRITE_HPP
#define WRITE_HPP

#include "message.hpp"

class Window;

class Write : public Thread
{
private:
	const char *name;
	int pipe;
public:
	Message<std::string> write; // get -- from Polytope
	Message<std::string> data; // get raw data from Window
	Write(int i, Window &gl, const char *n);
	virtual void init();
	virtual void call();
	virtual void done();
};

#endif