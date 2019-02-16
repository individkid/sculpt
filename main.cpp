/*
*    main.cpp start threads, allocate queues, glfw main loop
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

#include "write.hpp"
#include "window.hpp"
#include "polytope.hpp"
#include "read.hpp"

int main(int argc, char *argv[])
{
	int nfile = 0;
	int file[argc]; int rpipe[argc]; int wpipe[argc]; for (int i = 0; i < argc; i++) {
	file[nfile] = open(argv[i],O_RDWR);
	if (file[nfile] < 0) continue;
	char *name = new char[strlen(argv[i])+6]; strcpy(name,argv[i]); strcat(name,".fifo");
	mkfifo(name,0666);
	wpipe[nfile] = open(name,O_WRONLY);
	rpipe[nfile] = open(name,O_RDONLY);
	nfile++;}
	Write *write[nfile]; for (int i = 0; i < nfile; i++) write[i] = new Write(wpipe[i]);
	Window window(write,0);
	Polytope *polytope[nfile]; for (int i = 0; i < nfile; i++) polytope[i] = new Polytope(window,*write[i]);
	Read *read[nfile]; for (int i = 0; i < nfile; i++) read[i] = new Read(window,*polytope[i],rpipe[i]);
	window.wait();
	return 0;
}
