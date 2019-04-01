/*
*    system.cpp thread for producing sound
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

#include <lua.hpp>
#include "script.hpp"
#include "window.hpp"
#include "system.hpp"
#include "read.hpp"
#include "polytope.hpp"
#include "write.hpp"

Script::Script(int n) : nfile(n), rsp2read(new Message<Data*>*[n]),
	req2polytope(new Message<Data*>*[n]), req2write(new Message<Data*>*[n]),
	read2req(this), window2req(this), polytope2rsp(this), system2req(this),
	write2rsp(this), system2rsp(this)
{
}

void Script::connect(int i, Read *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    rsp2read[i] = &ptr->script2rsp;
}

void Script::connect(Window *ptr)
{
	rsp2window = &ptr->script2rsp;
}

void Script::connect(int i, Polytope *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    req2polytope[i] = &ptr->script2req;
}

void Script::connect(System *ptr)
{
	rsp2system = &ptr->script2rsp;
	req2system = &ptr->script2req;
}

void Script::connect(int i, Write *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    req2command[i] = &ptr->command2req;
    req2write[i] = &ptr->script2req;
}

void Script::call()
{
}
