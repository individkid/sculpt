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

#include "message.hpp"
#include "window.hpp"
#include "write.hpp"
#include "polytope.hpp"
#include "read.hpp"

int main(int argc, char *argv[])
{
	argc--; argv++;
	Window window(argc);
	Write *write[argc]; for (int i = 0; i < argc; i++) write[i] = new Write(i,window,argv[i]);
	Polytope *polytope[argc]; for (int i = 0; i < argc; i++) polytope[i] = new Polytope(i,window,*write[i]);
	Read *read[argc]; for (int i = 0; i < argc; i++) read[i] = new Read(i,window,*polytope[i],argv[i]);
	window.kill();
	for (int i = 0; i < argc; i++) write[i]->kill();
	for (int i = 0; i < argc; i++) polytope[i]->kill();
	for (int i = 0; i < argc; i++) read[i]->kill();
	return 0;
}
