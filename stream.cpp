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

Opcode Stream::get(int fd, Data *&data, Command *&command)
{
	Opcode opcode;
	if (read(fd,&opcode,sizeof(opcode)) != sizeof(opcode)) error("read failed",errno,__FILE__,__LINE__);
	switch (opcode) {
	case (ReadOp): {data = datas.get(); command = 0;
	if (read(fd,data,sizeof(data)) != sizeof(data)) error("read failed",errno,__FILE__,__LINE__);
	Symbol texts; texts.text = data->text; char *text = chars.get(texts.count); data->text = text;
	int size = texts.count*sizeof(*texts.text);
	if (read(fd,text,size) != size) error("read failed",errno,__FILE__,__LINE__);
	break;}
	default: error("unimplemented opcode",opcode,__FILE__,__LINE__);}
	return opcode;
}

void Stream::put(int fd, Opcode opcode, Data *data)
{
	if (write(fd,&opcode,sizeof(opcode)) != sizeof(opcode)) error("write failed",errno,__FILE__,__LINE__);
	switch (opcode) {
	case (ReadOp): {
	char *text = data->text; Symbol texts; texts.count = strlen(text)+1; data->text = texts.text;
	int size = texts.count*sizeof(*texts.text);
	if (write(fd,data,sizeof(data)) != sizeof(data)) error("write failed",errno,__FILE__,__LINE__);
	if (write(fd,text,size) != size) error("write failed",errno,__FILE__,__LINE__);
	Pools::datas.put(data); break;}
	default: error("unimplemented opcode",opcode,__FILE__,__LINE__);}
}

void Stream::put(int fd, Opcode opcode, Command *command)
{
	// TODO
}
