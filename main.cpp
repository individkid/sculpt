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
#include "file.hpp"
#include "write.hpp"
#include "polytope.hpp"
#include "read.hpp"
#include "script.hpp"
#include "system.hpp"

Window *window = 0;
int main(int argc, char *argv[])
{
	char *path = argv[0];
	argc--; argv++;

	File *file[argc]; for (int i = 0; i < argc; i++) file[i] = new File(argv[i]);
	Read *read[argc]; for (int i = 0; i < argc; i++) read[i] = new Read(i,file[i]);
	Write *write[argc]; for (int i = 0; i < argc; i++) write[i] = new Write(i,file[i]);
	System *system = new System(argc);
	Script *script = new Script(argc);
	Polytope *polytope = new Polytope(argc,argv[0]);
	window = new Window(argc);

	for (int i = 0; i < argc; i++) {read[i]->connect(polytope); polytope->connect(i,read[i]);}
	for (int i = 0; i < argc; i++) {read[i]->connect(system); system->connect(i,read[i]);}
	for (int i = 0; i < argc; i++) {read[i]->connect(script); script->connect(i,read[i]);}
	for (int i = 0; i < argc; i++) {read[i]->connect(window); window->connect(i,read[i]);}
	for (int i = 0; i < argc; i++) {write[i]->connect(script); script->connect(i,write[i]);}
	for (int i = 0; i < argc; i++) {write[i]->connect(window); window->connect(i,write[i]);}
	system->connect(script); script->connect(system);
	for (int i = 0; i < argc; i++) {polytope->connect(i,write[i]); write[i]->connect(polytope);}
	polytope->connect(script); script->connect(polytope);
	polytope->connect(window); window->connect(polytope);

	for (int i = 0; i < argc; i++) read[i]->exec();
	for (int i = 0; i < argc; i++) write[i]->exec();
	system->exec();
	script->exec();
	polytope->exec();
	window->exec();

	for (int i = 0; i < argc; i++) read[i]->kill();
	for (int i = 0; i < argc; i++) write[i]->kill();
	system->kill();
	script->kill();
	polytope->kill();
	window->kill();

	for (int i = 0; i < argc; i++) delete read[i];
	for (int i = 0; i < argc; i++) delete write[i];
	for (int i = 0; i < argc; i++) delete file[i];
	delete system;
	delete script;
	delete polytope;
	delete window;

	return 0;
}
