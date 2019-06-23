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
#include "file.hpp"

static Parse parse(__FILE__,__LINE__);

void Read::connect(Window *ptr)
{
	req2command = &ptr->read2req;
}

void Read::connect(Polytope *ptr)
{
	req2data = &ptr->read2req;
}

void Read::connect(System *ptr)
{
	req2sound = &ptr->read2req;
}

void Read::connect(Script *ptr)
{
	req2query = &ptr->read2req;
}

void Read::init()
{
	if (req2command == 0) error("unconnected req2command",0,__FILE__,__LINE__);
	if (req2data == 0) error("unconnected req2polytope",0,__FILE__,__LINE__);
	if (req2sound == 0) error("unconnected req2sound",0,__FILE__,__LINE__);
	if (req2query == 0) error("unconnected req2query",0,__FILE__,__LINE__);
}

void Read::call()
{
    Command *command; Data *data; Sound *sound; Query *query;
	while (command2rsp.get(command)) parse.put(command);
    while (data2rsp.get(data)) parse.put(data);
    while (sound2rsp.get(sound)) parse.put(sound);
	while (query2rsp.get(query)) parse.put(query);
	const char *str = buffer;
	while (parse.get(str,self,command,data,sound,query)) {
	if (command) req2command->put(command);
	if (data) req2data->put(data);
	if (sound) req2sound->put(sound);
	if (query) req2query->put(query);}
	int length = str-buffer;
	buffer = parse.postfix(buffer,length);
}

void Read::wait()
{
	char *temp = parse.setup(BUFFER_SIZE);
	int length = file->read(temp,BUFFER_SIZE-1);
	temp[length] = 0;
	buffer = parse.concat(buffer,temp);
}

void Read::done()
{
}

Read::Read(int s, File *f) : Thread(),
	command2rsp(this,"Read<-Data<-Command"),
	data2rsp(this,"Read<-Data<-Data"),
	sound2rsp(this,"Read<-Sound<-System"), 
	query2rsp(this,"Read<-Query<-Script"),
	self(s), file(f)
{
	buffer = parse.setup("");
}

Read::~Read()
{
	Command *command; Data *data; Sound *sound; Query *query;
	while (command2rsp.get(command)) parse.put(command);
    while (data2rsp.get(data)) parse.put(data);
    while (sound2rsp.get(sound)) parse.put(sound);
	while (query2rsp.get(query)) parse.put(query);
}
