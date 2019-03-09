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

Read::Read(int i, Window &gl, Polytope &r, const char *n) : Thread(), data(gl), read(r), name(n), file(-1), pipe(-1), self(i), fpos(0)
{
	gl.connect(i,this);
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
	// read to eof
	std::string cmdstr; int num; char chr;
	while ((num = ::read(file, &chr, 1)) == 1) {cmdstr += chr; fpos++;}
	if (num < 0 && errno != EINTR) error("read error",errno,__FILE__,__LINE__);
	while (cmdstr.size()) {
	int pre = prestr(cmdstr.substr(2,std::string::npos),"--");
	int len = 2+pre;
	std::string substr = cmdstr.substr(0,len);
	cmdstr = cmdstr.substr(len,std::string::npos);
	if (cmpstr(substr,"--test") > 0) data.data.put(substr); else read.read.put(substr);}
}

void Read::wait()
{
	int num; char chr; struct flock lock; struct stat size;
	// try to get writelock at eof to infinity, and block on read from pipe if successful and still at end, releasing after appending to file from pipe
	lock.l_start = fpos; lock.l_len = INFINITE; lock.l_type = F_WRLCK; lock.l_whence = SEEK_SET; num = fcntl(file,F_SETLK,&lock); lock.l_type = F_UNLCK;
	if (num < 0) {if (errno == EAGAIN) return; else error("fcntl failed",errno,__FILE__,__LINE__);}
	if (fstat(file, &size) < 0) error("fstat failed",errno,__FILE__,__LINE__);
	if (size.st_size > fpos) {if (fcntl(file,F_SETLK,&lock) < 0) error("fcntl failed",errno,__FILE__,__LINE__); else return;}
	while ((num = ::read(pipe, &chr, 1)) == 1) {fpos++; if (write(file, &chr, 1) < 0) error("write failed",errno,__FILE__,__LINE__);}
	if (num < 0 && errno != EINTR) error("read failed",errno,__FILE__,__LINE__);
	if (num == 0) {Thread::wait(); return;}
	if (fcntl(file,F_SETLK,&lock) < 0) error("fcntl failed",errno,__FILE__,__LINE__); else return;
	// otherwise wait for readlock and release when acquired
	lock.l_start = fpos; lock.l_len = 1; lock.l_type = F_RDLCK; lock.l_whence = SEEK_SET; num = fcntl(file,F_SETLKW,&lock); lock.l_type = F_UNLCK;
	if (fcntl(file,F_SETLK,&lock) < 0) error("fcntl failed",errno,__FILE__,__LINE__); else return;
}

void Read::done()
{
	if (close(file) < 0) error("close failed",errno,__FILE__,__LINE__);
	if (close(pipe) < 0) error("close failed",errno,__FILE__,__LINE__);
}
