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

Read::Read(int s, const char *n) : Thread(), name(n), file(-1), pipe(-1), self(s),
	fpos(0), mlen(0), glen(0), mnum(0), gnum(0),
	window2command2rsp(this), window2sync2rsp(this), window2mode2rsp(this),
	polytope2data2rsp(this,"Read<-Data<-Polytope"), system2data2rsp(this), script2data2rsp(this)
{
}

void Read::connect(Window *ptr)
{
	req2command2window = &ptr->read2command2req;
	req2sync2window = &ptr->read2sync2req;
	req2mode2window = &ptr->read2mode2req;
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
	// deallocate message responses
	get(window2sync2rsp);
    get(window2sync2rsp);
    get(window2mode2rsp);
    get(polytope2data2rsp);
    get(system2data2rsp);
    get(script2data2rsp);
	// read to eof
	char *cmdstr = parse.setup(""); int num; char chr; off_t pos = fpos;
	while ((num = ::read(file, &chr, 1)) == 1) {
	cmdstr = parse.concat(cmdstr,chr); fpos++;}
	if (num < 0 && errno != EINTR) error("read error",errno,__FILE__,__LINE__);
	while (*cmdstr) {
	int len = 0; if (cmdstr[len] == '-' && cmdstr[len+1] == '-') len += 2;
	while (cmdstr[len] && !(cmdstr[len] == '-' && cmdstr[len+1] == '-')) len++;
	const char *temp = cmdstr;
	char *substr = parse.prefix(temp,len);
	cmdstr = parse.postfix(cmdstr,len);
	// remember memory mapped commands
	if (len) {
	num = parse.literal(substr,"--matrix");
	if (num) {mpos = pos+num; mlen = strlen(substr)-num;}
	num = parse.literal(substr,"--global");
	if (num) {gpos = pos+num; glen = strlen(substr)-num;}}
	// update display from memory mapped commands
	Sync *mmap = read(mpos,mlen,mnum,PolytopeMode);
	if (mmap) put(*req2sync2window,mmap);
	Sync *gmap = read(gpos,glen,gnum,SessionMode);
	if (gmap) put(*req2sync2window,gmap);
    // send parsed messages
    Command *command = 0; Sync *sync = 0; Mode *mode = 0;
    Data *polytope = 0; Data *system = 0; Data *script = 0;
	parse.get(parse.cleanup(substr),self,command,sync,mode,polytope,system,script);
	put(*req2command2window,command);
	put(*req2sync2window,sync);
	put(*req2mode2window,mode);
	put(*req2data2polytope,polytope);
	put(*req2data2system,system);
	put(*req2data2script,script);}
	parse.cleanup(cmdstr);
}

void Read::wait()
{
	int num, len; char chr; struct flock lock; struct stat size; char *str;
	fd_set fds; struct timespec tv; sigset_t sm;
	FD_ZERO(&fds); FD_SET(pipe,&fds); tv.tv_sec = 0; tv.tv_nsec = 0;
	if (pthread_sigmask(SIG_SETMASK,0,&sm)) error ("cannot get mask",errno,__FILE__,__LINE__);
	sigdelset(&sm, SIGUSR1);
	// try to get writelock at eof to infinity
	lock.l_start = fpos; lock.l_len = INFINITE; lock.l_type = F_WRLCK; lock.l_whence = SEEK_SET;
	num = fcntl(file,F_SETLK,&lock); lock.l_type = F_UNLCK;
	if (num < 0 && errno != EAGAIN) error("fcntl failed",errno,__FILE__,__LINE__);
	if (num == 0) {
	if (fstat(file, &size) < 0) error("fstat failed",errno,__FILE__,__LINE__);
	// if successful and still at end
	if (size.st_size > fpos) {
	if (fcntl(file,F_SETLK,&lock) < 0) error("fcntl failed",errno,__FILE__,__LINE__); else return;}
	// block to read first character from pipe
	num = pselect(pipe+1,&fds,0,&fds,0,&sm);
	if (num < 0 && errno == EINTR) {
	if (fcntl(file,F_SETLK,&lock) < 0) error("fcntl failed",errno,__FILE__,__LINE__); else return;}
	if (num <= 0) error("pselect failed",errno,__FILE__,__LINE__);
	num = ::read(pipe, &chr, 1);
	if (num < 0) error("read failed",errno,__FILE__,__LINE__);
	if (num == 0) {
	if (fcntl(file,F_SETLK,&lock) < 0) error("fcntl failed",errno,__FILE__,__LINE__);
	Thread::wait(); return;}
	str = parse.setup("");
	str = parse.concat(str,chr);
	// read from pipe while it would not block
	while (1) {
	num = pselect(pipe+1,&fds,0,&fds,&tv,0);
	if (num < 0) error("pselect failed",errno,__FILE__,__LINE__);
	if (num == 0) break;
	num = ::read(pipe, &chr, 1);
	if (num != 1) error("read failed",errno,__FILE__,__LINE__);
	str = parse.concat(str,chr);}
	// append to file
	len = strlen(str);
	num = parse.literal(str,"--matrix");
	if (num && write(str+num,mpos,mlen,mnum)) len = 0;
	num = parse.literal(str,"--global");
	if (num && write(str+num,gpos,glen,gnum)) len = 0;
	if (len) {num = ::write(file,str,len);
	if (num != len) error("write failed",errno,__FILE__,__LINE__);}
	parse.cleanup(str);
	if (fcntl(file,F_SETLK,&lock) < 0) error("fcntl failed",errno,__FILE__,__LINE__); else return;}
	// wait for readlock and release when acquired
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

Sync *Read::read(off_t pos, int len, int &num, enum TargetMode target)
{
	if (len == 0) return 0;
	char *str = parse.setup(len+1);
	struct flock lock; lock.l_start = fpos; lock.l_len = 1; lock.l_type = F_RDLCK; lock.l_whence = SEEK_SET;
	int res = -1; while (res) {res = fcntl(file,F_SETLKW,&lock);
	if (res < 0 && errno != EINTR) error("flock failed",errno,__FILE__,__LINE__);} lock.l_type = F_UNLCK;
	if (lseek(file,pos,SEEK_SET) < 0) error("lseek failed",errno,__FILE__,__LINE__);
	if (::read(file,str,len) != len) error("read fail",errno,__FILE__,__LINE__);
	if (lseek(file,fpos,SEEK_SET) < 0) error("lseek failed",errno,__FILE__,__LINE__);
	if (fcntl(file,F_SETLK,&lock) < 0) error("fcntl failed",errno,__FILE__,__LINE__);
	Sync *sync; str[len] = 0; parse.get(str,self,target,sync);
	if (sync->number == num) {parse.put(sync); parse.cleanup(str); return sync;}
	num = sync->number; parse.cleanup(str); return sync;
}

int Read::write(const char *str, off_t pos, int len, int &num)
{
	if (len == 0) return 0;
	char *chs = parse.setup(str);
	int chl = strlen(chs);
	if (chl > len) {parse.cleanup(chs); return 0;}
	while (chl < len) {chs = parse.concat(chs," "); chl++;}
	number(chs,num);
	struct flock lock; lock.l_start = fpos; lock.l_len = 1; lock.l_type = F_WRLCK; lock.l_whence = SEEK_SET;
	int res = -1; while (res) {res = fcntl(file,F_SETLKW,&lock);
	if (res < 0 && errno != EINTR) error("flock failed",errno,__FILE__,__LINE__);} lock.l_type = F_UNLCK;
	if (lseek(file,pos,SEEK_SET) < 0) error("lseek failed",errno,__FILE__,__LINE__);
	if (::write(file,chs,chl) != chl) error("write fail",errno,__FILE__,__LINE__);
	if (lseek(file,fpos,SEEK_SET) < 0) error("lseek failed",errno,__FILE__,__LINE__);
	parse.cleanup(chs); return 1;
}

void Read::number(char *str, int &num)
{
	int val; int len = parse.number(str,val); num = val+1;
	char *chs = parse.string(num);
	int chl = strlen(chs);
	while (chl < len) {chs = parse.concat(" ",chs); chl++;}
	for (int i = 0; i < len; i++) str[i] = chs[i]; parse.cleanup(chs);
}
