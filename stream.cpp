/*
*    stream.cpp functions for converting pipe bytes to Command
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

#include <unistd.h>
#include "stream.hpp"
extern "C" {
#include "haskell.h"
}

void Stream::get(int fd, Command *&command)
{
	// TODO
}

void Stream::get(int fd, Update *&update)
{
	// TODO
}

void Stream::get(int fd, Render *&render)
{
	// TODO
}

void Stream::get(int fd, Manip *&manip)
{
	// TODO
}

void Stream::get(int fd, Query *&query)
{
	// TODO
}

void Stream::get(int fd, Data *&data)
{
	data = Pools::datas.get();
	exOpcode(fd,FileOp); data->file = rdInt(fd);
	exOpcode(fd,PlaneOp); data->plane = rdInt(fd);
	exOpcode(fd,ConfOp); data->conf = (Configure)rdConfigure(fd);
	switch (data->conf) {
	// TODO
	case (TestConf): exOpcode(fd,TextOp); get(fd,data->text); break;
	default: error("unimplemented configure",data->conf,__FILE__,__LINE__);}
}

void Stream::get(int fd, char *&text)
{
	int count = rdInt(fd);
	text = Pools::chars.get(count);
	rdChars(fd,count,text);
	if (text[count-1]) error("invalid text",0,__FILE__,__LINE__);
}

void Stream::put(int fd, char *text)
{
	int count = strlen(text)+1;
	wrInt(fd,count);
	wrChars(fd,count,text);
}

Opcode Stream::get(int fd, Data *&data, Query *&query, Manip *&manip, Command *&command)
{
	data = 0; query = 0; manip = 0; command = 0;
	Opcode opcode = (Opcode)rdOpcode(fd);
	switch (opcode) {
	// TODO for responses with data read into the struct pointed to by rdPointer
	case (ReadOp): data = (struct Data *)rdPointer(fd); break;
	case (WriteOp): get(fd,data); break;
	case (QueryOp): query = (struct Query *)rdPointer(fd); /*TODO fill query with response*/ break;
	case (CommandOp): get(fd,command); break;
	default: error("unimplemented opcode",opcode,__FILE__,__LINE__);}
	return opcode;
}

void Stream::put(int fd, Opcode opcode, Command *command)
{
	// TODO
}

void Stream::put(int fd, Opcode opcode, Manip *manip)
{
	// TODO
}

void Stream::put(int fd, Opcode opcode, Query *query)
{
	wrOpcode(fd,opcode); wrPointer(fd,query);
	// TODO write query arguments
}

void Stream::put(int fd, Opcode opcode, Data *data)
{
	int rsp; switch (opcode) {
	case (ReadOp): rsp = 0; break;
	case (WriteOp): rsp = 1; break;
	default: error("unimplemented opcode",opcode,__FILE__,__LINE__);}
	wrOpcode(fd,opcode); wrPointer(fd,(rsp?0:data));
	wrOpcode(fd,FileOp); wrInt(fd,data->file);
	wrOpcode(fd,PlaneOp); wrInt(fd,data->plane);
	wrOpcode(fd,ConfOp); wrConfigure(fd,data->conf);
	switch (data->conf) {
	// 	TODO
	case (TestConf): wrOpcode(fd,TextOp); put(fd,data->text); break;
	default: error("unimplemented configure",data->conf,__FILE__,__LINE__);}
	if (rsp) {switch (data->conf) {
	// TODO
	case (TestConf): Pools::chars.put(strlen(data->text)+1,data->text); break;
	default: error("unimplemented configure",data->conf,__FILE__,__LINE__);}
	Pools::datas.put(data);}
}
