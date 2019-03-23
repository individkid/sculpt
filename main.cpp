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
#include "script.hpp"
#include "system.hpp"

Window *window = 0;
int main(int argc, char *argv[])
{
	argc--; argv++;
	window = new Window(argc);
	Script *script = new Script(argc);
	System *system = new System(argc);
	Write *write[argc]; for (int i = 0; i < argc; i++) write[i] = new Write(i,argv[i]);
	Polytope *polytope[argc]; for (int i = 0; i < argc; i++) polytope[i] = new Polytope(i);
	Read *read[argc]; for (int i = 0; i < argc; i++) read[i] = new Read(i,argv[i]);
	// Read->Command->Window Read->Data->Window
	for (int i = 0; i < argc; i++) {read[i]->connect(window); window->connect(i,read[i]);}
	// Read->Data->Polytope
	for (int i = 0; i < argc; i++) {read[i]->connect(polytope[i]); polytope[i]->connect(read[i]);}
	// Read->Data->System
	for (int i = 0; i < argc; i++) {read[i]->connect(system); system->connect(i,read[i]);}
	// Read->Data->Script
	for (int i = 0; i < argc; i++) {read[i]->connect(script); script->connect(i,read[i]);}
	// Window->Data->Write
	for (int i = 0; i < argc; i++) {window->connect(i,write[i]); write[i]->connect(window);}
	// Window->Data->Polytope
	for (int i = 0; i < argc; i++) {window->connect(i,polytope[i]); polytope[i]->connect(window);}
	// Window->Invoke->Script
	window->connect(script); script->connect(window);
	// Polytope->Data->Write
	for (int i = 0; i < argc; i++) {polytope[i]->connect(write[i]); write[i]->connect(polytope[i]);}
	// Polytope->Command->Window
	for (int i = 0; i < argc; i++) {polytope[i]->connect(window); window->connect(i,polytope[i]);}
	// Script->Question->Polytope
	for (int i = 0; i < argc; i++) {script->connect(i,polytope[i]); polytope[i]->connect(script);}
	// System->Invoke->Script
	system->connect(script); script->connect(system);
	window->run(); script->kill(); system->kill();
	for (int i = 0; i < argc; i++) write[i]->kill();
	for (int i = 0; i < argc; i++) polytope[i]->kill();
	for (int i = 0; i < argc; i++) read[i]->kill();
	return 0;
}
