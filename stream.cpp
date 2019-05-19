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

void Stream::get(int fd, Command *&command)
{
	command = Pools::commands.get();
	if (read(fd,command,sizeof(*command)) != sizeof(*command)) error("read failed",errno,__FILE__,__LINE__);
	for (int i = 0; i < Fields; i++) get(fd,command->update[i]); get(fd,command->render);
	if (command->redraw) get(fd,command->redraw);
	if (command->pierce) get(fd,command->pierce);
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
	data = datas.get();
	if (read(fd,data,sizeof(data)) != sizeof(data)) error("read failed",errno,__FILE__,__LINE__);
	switch (data->conf) {
	case (TestConf): get(fd,data->text); break;
	default: error("unimplemented configure",data->conf,__FILE__,__LINE__);}
}

void Stream::get(int fd, char *&text)
{
	Symbol texts; texts.text = text; text = chars.get(texts.count);
	int size = texts.count*sizeof(*texts.text);
	if (read(fd,text,size) != size) error("read failed",errno,__FILE__,__LINE__);
}

int Stream::put(char *&text)
{
	Symbol texts; texts.count = strlen(text)+1; text = texts.text;
	return texts.count*sizeof(*texts.text);
}

void Stream::put(int fd, int size, char *&text)
{
	if (write(fd,text,size) != size) error("write failed",errno,__FILE__,__LINE__);
}

Opcode Stream::get(int fd, Data *&data, Command *&command)
{
	Opcode opcode;
	if (read(fd,&opcode,sizeof(opcode)) != sizeof(opcode)) error("read failed",errno,__FILE__,__LINE__);
	switch (opcode) {
	case (ReadOp): case (WriteOp): case (ScriptOp): case (WindowOp):
	get(fd,data); command = 0; break;
	case (CommandOp): data = 0; get(fd,command); break;
	default: error("unimplemented opcode",opcode,__FILE__,__LINE__);}
	return opcode;
}

void Stream::put(int fd, Opcode opcode, Data *data)
{
	int size0;
	if (write(fd,&opcode,sizeof(opcode)) != sizeof(opcode)) error("write failed",errno,__FILE__,__LINE__);
	switch (data->conf) {
	case (TestConf): size0 = put(data->text); break;
	default: error("unimplemented opcode",data->conf,__FILE__,__LINE__);}
	if (write(fd,data,sizeof(data)) != sizeof(data)) error("write failed",errno,__FILE__,__LINE__);
	switch (data->conf) {
	case (TestConf): put(fd,size0,data->text); break;
	default: error("unimplemented opcode",data->conf,__FILE__,__LINE__);}
	Pools::datas.put(data);
}

void Stream::put(int fd, Opcode opcode, Command *command)
{
	// TODO
}
