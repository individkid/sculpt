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

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <signal.h>

#include "read.hpp"
#include "window.hpp"
#include "polytope.hpp"
#include "system.hpp"
#include "script.hpp"
#include "parse.hpp"

static Parse parse(__FILE__,__LINE__);

void Read::connect(Window *ptr)
{
	req2command = &ptr->command2req;
	req2window = &ptr->read2req;
}

void Read::connect(Polytope *ptr)
{
	req2polytope = &ptr->read2req;
}

void Read::connect(System *ptr)
{
	req2sound = &ptr->sound2req;
	req2system = &ptr->read2req;
}

void Read::connect(Script *ptr)
{
	req2script = &ptr->read2req;
}

void Read::init()
{
	File::init();
	if (req2command == 0) error("unconnected req2command",0,__FILE__,__LINE__);
	if (req2window == 0) error("unconnected req2window",0,__FILE__,__LINE__);
	if (req2polytope == 0) error("unconnected req2polytope",0,__FILE__,__LINE__);
	if (req2sound == 0) error("unconnected req2sound",0,__FILE__,__LINE__);
	if (req2system == 0) error("unconnected req2system",0,__FILE__,__LINE__);
	if (req2script == 0) error("unconnected req2script",0,__FILE__,__LINE__);
}

void Read::call()
{
    Command *command; Data *window; Data *polytope;
    Sound *sound; Data *system; Data *script;
	while (command2rsp.get(command)) parse.put(command);
	while (window2rsp.get(window)) parse.put(window);
    while (polytope2rsp.get(polytope)) parse.put(polytope);
    while (sound2rsp.get(sound)) parse.put(sound);
    while (system2rsp.get(system)) parse.put(system);
    while (script2rsp.get(script)) parse.put(script);
	char *str = File::read((Pools*)&parse);
	parse.get(str,self,command,window,polytope,sound,system,script);
	if (command) req2command->put(command);
	if (window) req2window->put(window);
	if (polytope) req2polytope->put(polytope);
	if (sound) req2sound->put(sound);
	if (system) req2system->put(system);
	if (script) req2script->put(script);
	parse.cleanup(str);
}

void Read::wait()
{
	File::wait();
}

void Read::done()
{
	File::done();
}

Read::Read(int s, const char *n) : File(n),
	command2rsp(this,"Read<-Data<-Command"), window2rsp(this,"Read<-Data<-Window"),
	polytope2rsp(this,"Read<-Data<-Polytope"), sound2rsp(this,"Read<-Sound<-System"),
	system2rsp(this,"Read<-Data<-System"), script2rsp(this,"Read<-Data<-Script"),
	self(s)
{
}

Read::~Read()
{
	Command *command; Sound *sound; Data *data;
	while (command2rsp.get(command)) parse.put(command);
	while (window2rsp.get(data)) parse.put(data);
    while (polytope2rsp.get(data)) parse.put(data);
    while (sound2rsp.get(sound)) parse.put(sound);
    while (system2rsp.get(data)) parse.put(data);
    while (script2rsp.get(data)) parse.put(data);
}
