/*
*    read.cpp thread for reading and appending file
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

#include "read.hpp"

void Read::init()
{
	char *pname = new char[strlen(name)+6]; strcpy(pname,name); strcat(pname,".fifo");
	if ((file = open(name,O_RDWR)) < 0) error("cannot open",name,__FILE__,__LINE__);
	if (mkfifo(pname,0666) < 0 && errno != EEXIST) error("cannot open",pname,__FILE__,__LINE__);
	if ((pipe = open(pname,O_RDONLY)) < 0) error("cannot open",pname,__FILE__,__LINE__);
	message("open",pname,__FILE__,__LINE__);
}

void Read::call()
{
}
