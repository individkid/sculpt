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

#include "system.hpp"
#include "read.hpp"
#include "script.hpp"

System::System(int n) : nfile(n), rsp2data2read(new Message<Data*>*[n]),
	read2data2req(this), script2invoke2rsp(this)
{
}

void System::connect(int i, Read *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    rsp2data2read[i] = &ptr->system2data2rsp;
}

void System::connect(Script *ptr)
{
	req2invoke2script = &ptr->system2invoke2req;
}

void System::call()
{
}
