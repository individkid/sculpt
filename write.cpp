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

static Power<char> chars(__FILE__,__LINE__);

void Write::connect(Polytope *ptr)
{
	rsp2polytope = &ptr->write2rsp;
}

void Write::connect(Script *ptr)
{
	rsp2script = &ptr->write2rsp;
}

void Write::connect(Window *ptr)
{
	rsp2window = &ptr->write2rsp;
}

void Write::init()
{
	if (rsp2polytope == 0) error("unconnected rsp2polytope",0,__FILE__,__LINE__);
	if (rsp2script == 0) error("unconnected rsp2script",0,__FILE__,__LINE__);
	if (rsp2window == 0) error("unconnected rsp2window",0,__FILE__,__LINE__);
}

void Write::call()
{
	Data *data;
	while (polytope2req.get(data)) {/*TODO unparse and send*/ rsp2polytope->put(data);}
	while (script2req.get(data)) {/*TODO unparse and send*/ rsp2script->put(data);}
	while (window2req.get(data)) {/*TODO unparse and send*/ rsp2window->put(data);}
}

void Write::done()
{
	Data *data;
	while (polytope2req.get(data)) rsp2polytope->put(data);
	while (script2req.get(data)) rsp2script->put(data);
	while (window2req.get(data)) rsp2window->put(data);
}

Write::Write(int i, File *f) : Thread(),
	rsp2polytope(0), rsp2script(0), rsp2window(0),
	polytope2req(this,"Polytope->Data->Write"), script2req(this,"Script->Data->Write"),
	window2req(this,"Window->Data->Write"), self(i), file(f)
{
}
