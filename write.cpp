/*
*    write.cpp thread for writing to pipe
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

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "write.hpp"
#include "window.hpp"
#include "polytope.hpp"
#include "script.hpp"

void Write::connect(Polytope *ptr)
{
	rsp2polytope = &ptr->write2rsp;
}

void Write::connect(Window *ptr)
{
	rsp2window = &ptr->write2rsp;
}

void Write::connect(Script *ptr)
{
	rsp2script = &ptr->write2rsp;
}

void Write::init()
{
	if (rsp2polytope == 0) error("unconnected rsp2polytope",0,__FILE__,__LINE__);
	if (rsp2window == 0) error("unconnected rsp2window",0,__FILE__,__LINE__);
	if (rsp2script == 0) error("unconnected rsp2script",0,__FILE__,__LINE__);
}

void Write::call()
{
	State *state;
	while (polytope2req.get(state)) {/*TODO unparse and send*/ rsp2polytope->put(state);}
	while (window2req.get(state)) {/*TODO unparse and send*/ rsp2window->put(state);}
	while (script2req.get(state)) {/*TODO unparse and send*/ rsp2script->put(state);}
}

void Write::done()
{
	State *state;
	while (polytope2req.get(state)) rsp2polytope->put(state);
	while (window2req.get(state)) rsp2window->put(state);
	while (script2req.get(state)) rsp2script->put(state);
}

Write::Write(int i, File *f) : Thread(),
	rsp2polytope(0), rsp2window(0),
	polytope2req(this,"Polytope->State->Write"),
	window2req(this,"Window->State->Write"),
	script2req(this,"Script->State->Write"),
	self(i), file(f)
{
}
