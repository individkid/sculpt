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
	command = Pools::commands.get();
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
	Pools::chars.put(count,text);
}

Opcode Stream::get(int fd, Data *&data, Command *&command)
{
	Opcode opcode = (Opcode)rdOpcode(fd);
	switch (opcode) {
	case (ReadOp): case (WriteOp): case (ScriptOp): case (WindowOp):
	get(fd,data); command = 0; break;
	case (CommandOp): data = 0; get(fd,command); break;
	default: error("unimplemented opcode",opcode,__FILE__,__LINE__);}
	return opcode;
}

void Stream::put(int fd, Opcode opcode, Data *data)
{
	wrOpcode(fd,opcode);
	wrOpcode(fd,FileOp); wrInt(fd,data->file);
	wrOpcode(fd,PlaneOp); wrInt(fd,data->plane);
	wrOpcode(fd,ConfOp); wrConfigure(fd,data->conf);
	switch (data->conf) {
	// 	TODO
	case (TestConf): wrOpcode(fd,TextOp); put(fd,data->text); break;
	default: error("unimplemented configure",data->conf,__FILE__,__LINE__);}
	Pools::datas.put(data);
}

void Stream::put(int fd, Opcode opcode, Command *command)
{
	// TODO
}
