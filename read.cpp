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

#include "read.hpp"
extern "C" {
#include "callback.h"
}
#include "file.hpp"
#include "script.hpp"

static Pool<Query> queries(__FILE__,__LINE__);
static Power<char> chars(__FILE__,__LINE__);

void Read::connect(Script *ptr)
{
	req2script = &ptr->read2req;
}

void Read::init()
{
	if (req2script == 0) error("unconnected req2script",0,__FILE__,__LINE__);
}

void Read::call()
{
	Query *query;
	while (script2rsp.get(query)) {
	chars.put(strlen(query->chars)+1,query->chars);
	queries.put(query);}
	while ((length > 0 && !file->ready()) || found > 1) {
	int len = 0;
	while (buffer[len] != 0 && (len == 0 || buffer[len] != '-' || buffer[len+1] != '-')) len++;
    query = queries.get();
    query->given = CharsGiv;
    query->file = self;
    query->chars = chars.get(len+1);
    strncpy(query->chars,buffer,len);
    query->chars[len] = 0;
	req2script->put(query);
	buffer = ::postfix(chars,buffer,len);
	length -= len;
	if (found > 0) found--;}
}

void Read::wait()
{
	char temp[BUFFER_SIZE+1];
	int len = file->read(temp,BUFFER_SIZE);
	temp[len] = 0;
	const char *tmp = temp;
	buffer = ::concat(chars,buffer,tmp);
	length += len;
	int count = 0;
	for (int i = 0; i < len; i++)
	if (temp[i] != '-') count = 0;
	else if (count == 1) {count = 0; found++;}
	else count++;
}

void Read::done()
{
}

Read::Read(int s, File *f) : Thread(),
	script2rsp(this,"Read<-Query<-Script"),
	self(s), file(f)
{
	buffer = ::setup(chars,"");
	length = found = 0;
}

Read::~Read()
{
	Query *query;
	while (script2rsp.get(query)) {
	chars.put(strlen(query->chars)+1,query->chars);
	queries.put(query);}
}
