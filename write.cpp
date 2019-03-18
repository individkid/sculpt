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

#include "write.hpp"
#include "window.hpp"
#include "polytope.hpp"

Write::Write(int i, const char *n) : Thread(), name(n), pipe(-1),
	window2data2req(this), polytope2data2req(this)
{
}

void Write::connect(class Window *ptr)
{
	rsp2data2window = &ptr->write2data2rsp;
}

void Write::connect(class Polytope *ptr)
{
	rsp2data2polytope = &ptr->write2data2rsp;
}

void Write::init()
{
	char *pname = new char[strlen(name)+6]; strcpy(pname,name); strcat(pname,".fifo");
	if (mkfifo(pname,0666) < 0 && errno != EEXIST) error("cannot open",pname,__FILE__,__LINE__);
	if ((pipe = open(pname,O_WRONLY)) < 0) error("cannot open",pname,__FILE__,__LINE__);
}

void Write::call()
{
    // read from write and data; write to pipe
}

void Write::done()
{
	if (close(pipe) < 0) error("close failed",errno,__FILE__,__LINE__);
}
