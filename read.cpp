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

#include "read.hpp"
#include "window.hpp"
#include "polytope.hpp"
#include "system.hpp"
#include "script.hpp"

static Pool<Data> datas;
static Power<char> chars;

Command *parseCommand(const char *&ptr, int file);
void unparseCommand(Command *command);

void Read::parse(const char *ptr, int file)
{
	int len;
	len = len = literal(ptr,"--command");
	if (len) {ptr += len;
	Command *command = parseCommand(ptr,file);
	if (command) req2command2window->put(command);}
	len = literal(ptr,"--test");
	if (len) {ptr += len;
	Data *data = datas.get(); data->conf = TestConf; data->file = file;
	len = 0; while (ptr[len]) if (ptr[len] == '-' && ptr[len+1] == '-') break; else len++;
	data->text = prefix(chars,ptr,len+1); data->text[len] = 0;
	req2data2polytope->put(data);}
}

void Read::unparse(Data *data)
{
	if (data->conf == TestConf) chars.put(strlen(data->text)+1,data->text);
	datas.put(data);
}

void Read::unparse(Command *command)
{
	unparseCommand(command);
}

Read::Read(int s, const char *n) : Thread(), name(n), file(-1), pipe(-1), self(s), fpos(0),
	window2command2rsp(this), window2data2rsp(this),
	polytope2data2rsp(this,"Read<-Data<-Polytope"), system2data2rsp(this), script2data2rsp(this)
{
	int i = 0;
	thread2data2rsp[i++] = &window2data2rsp;
	thread2data2rsp[i++] = &polytope2data2rsp;
	thread2data2rsp[i++] = &system2data2rsp;
	thread2data2rsp[i++] = &script2data2rsp;
	thread2data2rsp[i] = 0;
}

void Read::connect(Window *ptr)
{
	req2command2window = &ptr->read2command2req;
	req2data2window = &ptr->read2data2req;
}

void Read::connect(Polytope *ptr)
{
	req2data2polytope = &ptr->read2data2req;
}

void Read::connect(System *ptr)
{
	req2data2system = &ptr->read2data2req;
}

void Read::connect(Script *ptr)
{
	req2data2script = &ptr->read2data2req;
}

void Read::init()
{
	char *pname = new char[strlen(name)+6]; strcpy(pname,name); strcat(pname,".fifo");
	if ((file = open(name,O_RDWR)) < 0) error("cannot open",name,__FILE__,__LINE__);
	if (mkfifo(pname,0666) < 0 && errno != EEXIST) error("cannot open",pname,__FILE__,__LINE__);
	if ((pipe = open(pname,O_RDONLY)) < 0) error("cannot open",pname,__FILE__,__LINE__);
}

void Read::call()
{
    Command *command; Data *data;
    while (window2command2rsp.get(command)) unparse(command);
    for (int i = 0; thread2data2rsp[i]; i++)
    while (thread2data2rsp[i]->get(data)) unparse(data);
	// read to eof
	char *cmdstr = setup(chars,""); int num; char chr;
	while ((num = ::read(file, &chr, 1)) == 1) {
	cmdstr = concat(chars,cmdstr,chr); fpos++;}
	if (num < 0 && errno != EINTR) error("read error",errno,__FILE__,__LINE__);
	while (*cmdstr) {
	int len = 0; if (cmdstr[len] == '-' && cmdstr[len+1] == '-') len += 2;
	while (cmdstr[len] && !(cmdstr[len] == '-' && cmdstr[len+1] == '-')) len++;
	char *substr = prefix(chars,cmdstr,len);
	cmdstr = postfix(chars,cmdstr,len);
	parse(cleanup(chars,substr),self);}
}

void Read::wait()
{
	int num; char chr; struct flock lock; struct stat size;
	// try to get writelock at eof to infinity, and block on read from pipe if successful and still at end, releasing after appending to file from pipe
	lock.l_start = fpos; lock.l_len = INFINITE; lock.l_type = F_WRLCK; lock.l_whence = SEEK_SET;
	num = fcntl(file,F_SETLK,&lock); lock.l_type = F_UNLCK;
	if (num < 0 && errno != EAGAIN) error("fcntl failed",errno,__FILE__,__LINE__);
	if (num < 0) {
	if (fstat(file, &size) < 0) error("fstat failed",errno,__FILE__,__LINE__);
	if (size.st_size > fpos) {
	if (fcntl(file,F_SETLK,&lock) < 0) error("fcntl failed",errno,__FILE__,__LINE__); else return;}
	while ((num = ::read(pipe, &chr, 1)) == 1) {fpos++;
	if (write(file, &chr, 1) < 0) error("write failed",errno,__FILE__,__LINE__);}
	if (num < 0 && errno != EINTR) error("read failed",errno,__FILE__,__LINE__);
	if (num == 0) {Thread::wait(); return;}
	if (fcntl(file,F_SETLK,&lock) < 0) error("fcntl failed",errno,__FILE__,__LINE__); else return;}
	// otherwise wait for readlock and release when acquired
	lock.l_start = fpos; lock.l_len = 1; lock.l_type = F_RDLCK; lock.l_whence = SEEK_SET;
	num = fcntl(file,F_SETLKW,&lock); lock.l_type = F_UNLCK;
	if (num < 0 && errno != EINTR) error("fcntl failed",errno,__FILE__,__LINE__);
	if (fcntl(file,F_SETLK,&lock) < 0) error("fcntl failed",errno,__FILE__,__LINE__); else return;
}

void Read::done()
{
	if (close(file) < 0) error("close failed",errno,__FILE__,__LINE__);
	if (close(pipe) < 0) error("close failed",errno,__FILE__,__LINE__);
}
