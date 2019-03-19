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

#include "script.hpp"
#include "window.hpp"
#include "system.hpp"
#include "read.hpp"
#include "polytope.hpp"
#include "write.hpp"

Script::Script(int n) : nfile(n), rsp2data2read(new Message<Data*>*[n]),
	req2question2polytope(new Message<Question*>*[n]), req2data2write(new Message<Data*>*[n]),
	read2data2req(this), window2invoke2req(this), polytope2question2rsp(this), system2invoke2req(this),
	write2data2rsp(this), system2question2rsp(this)
{
}

void Script::connect(int i, Read *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    rsp2data2read[i] = &ptr->script2data2rsp;
}

void Script::connect(Window *ptr)
{
	rsp2invoke2window = &ptr->script2invoke2rsp;
}

void Script::connect(int i, Polytope *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    req2question2polytope[i] = &ptr->script2question2req;
}

void Script::connect(System *ptr)
{
	rsp2invoke2system = &ptr->script2invoke2rsp;
	req2question2system = &ptr->script2question2req;
}

void Script::connect(int i, Write *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    req2data2write[i] = &ptr->script2data2req;
}

void Script::call()
{
}
