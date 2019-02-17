/*
*    polytope.hpp thread for sample and classify
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

#ifndef POLYTOPE_HPP
#define POLYTOPE_HPP

#include "message.hpp"

class Window;
class Write;

class Polytope : public Thread
{
private:
	Window &window; // send Command to Window
	Write &write; // send -- to Write
public:
	Message<char[STRING_ARRAY_SIZE]> read; // get -- from Read
	Message<Command> response; // get Command from Window
	Message<Action> action; // get Action from Window
	Polytope(int i, Window &gl, Write &w);
	virtual void call();
};

#endif
