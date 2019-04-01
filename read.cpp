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
	command2rsp(this), window2rsp(this), polytope2rsp(this,"Read<-Data<-Polytope"),
	system2rsp(this), script2rsp(this)
{
}

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
	req2system = &ptr->read2req;
}

void Read::connect(Script *ptr)
{
	req2script = &ptr->read2req;
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
	get(command2rsp);
	get(window2rsp);
    get(polytope2rsp);
    get(system2rsp);
    get(script2rsp);
	off_t pos = fpos;
	char *str = read();
	while (1) {
	char *dds = split(str);
	sync(dds,"--matrix",pos,mpos,mlen,mnum,MatrixConf);
	sync(dds,"--global",pos,gpos,glen,gnum,GlobalConf);
	if (*dds == 0) {parse.cleanup(dds); break;}
    Command *command = 0; Data *window = 0; Data *polytope = 0; Data *system = 0; Data *script = 0;
	parse.get(parse.cleanup(dds),self,command,window,polytope,system,script);
	put(*req2command,command);
	put(*req2window,window);
	put(*req2polytope,polytope);
	put(*req2system,system);
	put(*req2script,script);}
	parse.cleanup(str);
}

void Read::wait()
{
	if (trywrlck()) {
	if (race()) {unwrlck(); return;}
	char *str = parse.setup("");
	if (intr()) {unwrlck(); return;}
	while (check()) if (!read(str)) {unwrlck(); Thread::wait(); return;}
	if (sync(str,"--matrix",mpos,mlen,mnum)) {parse.cleanup(str); unwrlck(); return;}
	if (sync(str,"--global",gpos,glen,gnum)) {parse.cleanup(str); unwrlck(); return;}
	write(str); parse.cleanup(str); unwrlck(); return;}
	if (check()) return;
	if (getrdlck()) unrdlck();
}

void Read::done()
{
	if (close(file) < 0) error("close failed",errno,__FILE__,__LINE__);
	if (close(pipe) < 0) error("close failed",errno,__FILE__,__LINE__);
}

// read to end of file
char *Read::read()
{
	char *str = parse.setup(""); int num; char chr;
	if (lseek(file,fpos,SEEK_SET) < 0) error("lseek failed",errno,__FILE__,__LINE__);
	while ((num = ::read(file, &chr, 1)) == 1) {
	str = parse.concat(str,chr); fpos++;}
	if (num < 0 && errno != EINTR) error("read error",errno,__FILE__,__LINE__);
	return str;
}

// split string on double dash
char *Read::split(char *&str)
{
	int len = 0; if (str[len] == '-' && str[len+1] == '-') len += 2;
	while (str[len] && !(str[len] == '-' && str[len+1] == '-')) len++;
	const char *tmp = str;
	char *sub = parse.prefix(tmp,len);
	str = parse.postfix(str,len);
	return sub;
}

void Read::sync(const char *str, const char *pat, off_t pos, off_t &sav, int &len, int &num, enum Configure conf)
{
	// update sync struct location if read from pipe
	int ddl = parse.literal(str,pat);
	if (ddl) {sav = pos+ddl; len = strlen(str)-ddl;}
	// send sync struct from middle of file if it has the wrong sequence number
	if (len == 0) return;
	char *sstr = parse.setup(len); struct flock lock; int res = -1;
	lock.l_start = sav; lock.l_len = len; lock.l_type = F_RDLCK; lock.l_whence = SEEK_SET;
	while (res) {res = fcntl(file,F_SETLKW,&lock);
	if (res < 0 && errno != EINTR) error("flock failed",errno,__FILE__,__LINE__);}
	if (lseek(file,sav,SEEK_SET) < 0) error("lseek failed",errno,__FILE__,__LINE__);
	if (::read(file,sstr,len) != len) error("read fail",errno,__FILE__,__LINE__);
	lock.l_type = F_UNLCK;
	if (fcntl(file,F_SETLK,&lock) < 0) error("fcntl failed",errno,__FILE__,__LINE__);
	Data *data; sstr[len] = 0; parse.get(sstr,self,conf,data);
	if (data->seqnum == num) {parse.put(data); parse.cleanup(sstr); return;}
	num = data->seqnum; parse.cleanup(sstr); put(*req2window,data);
}

// nonblocking try to get write lock at end of file to infinity
int Read::trywrlck()
{
	int num; struct flock lock;
	lock.l_start = fpos; lock.l_len = INFINITE; lock.l_type = F_WRLCK; lock.l_whence = SEEK_SET;
	num = fcntl(file,F_SETLK,&lock); lock.l_type = F_UNLCK;
	if (num < 0 && errno != EAGAIN) error("fcntl failed",errno,__FILE__,__LINE__);
	return (num == 0);
}

// check if already read to end of file
int Read::race()
{
	struct stat size;
	if (fstat(file, &size) < 0) error("fstat failed",errno,__FILE__,__LINE__);
	return (size.st_size > fpos);
}

// release infinite lock at end of file
void Read::unwrlck()
{
	struct flock lock;
	lock.l_start = fpos; lock.l_len = INFINITE; lock.l_type = F_UNLCK; lock.l_whence = SEEK_SET;
	if (fcntl(file,F_SETLK,&lock) < 0) error("fcntl failed",errno,__FILE__,__LINE__);
}

// block until read from pipe would not block
int Read::intr()
{
	fd_set fds; sigset_t sm; FD_ZERO(&fds); FD_SET(pipe,&fds);
	if (pthread_sigmask(SIG_SETMASK,0,&sm)) error ("cannot get mask",errno,__FILE__,__LINE__);
	sigdelset(&sm, SIGUSR1);
	int num = pselect(pipe+1,&fds,0,&fds,0,&sm);
	if (num < 0 && errno == EINTR) return 1;
	return 0;
}

// check if read from pipe would block
int Read::check()
{
	fd_set fds; struct timespec tv;
	FD_ZERO(&fds); FD_SET(pipe,&fds); tv.tv_sec = 0; tv.tv_nsec = 0;
	int num = pselect(pipe+1,&fds,0,&fds,&tv,0);
	if (num < 0) error("pselect failed",errno,__FILE__,__LINE__);
	return (num != 0);
}

// read from pipe and append to string
int Read::read(char *&str)
{
	char chr; int num = ::read(pipe, &chr, 1);
	if (num < 0) error("read failed",errno,__FILE__,__LINE__);
	if (num == 0) return 0;
	str = parse.concat(str,chr);
	return 1;
}

int Read::sync(const char *str, const char *pat, off_t pos, int len, int &num)
{
	// extend sync string to expected length
	int pre = parse.literal(str,pat);
	if (pre == 0 && len == 0) return 0;
	char *dstr = parse.setup(str+pre);
	int dlen = strlen(dstr);
	if (dlen > len) {parse.cleanup(dstr); return 0;}
	while (dlen < len) {dstr = parse.concat(dstr," "); dlen++;}
	// change sequence number in sync string
	int val; int flen = parse.number(dstr,val); num = val+1;
	char *nstr = parse.string(num);
	int nlen = strlen(nstr);
	while (nlen < flen) {nstr = parse.concat(" ",nstr); nlen++;}
	for (int i = 0; i < nlen; i++) dstr[i] = nstr[i]; parse.cleanup(nstr);
	// write sync string to middle of file
	struct flock lock; lock.l_start = pos; lock.l_len = len; lock.l_type = F_WRLCK; lock.l_whence = SEEK_SET;
	int res = -1;
	while (res) {res = fcntl(file,F_SETLKW,&lock);
	if (res < 0 && errno != EINTR) error("flock failed",errno,__FILE__,__LINE__);}
	if (lseek(file,pos,SEEK_SET) < 0) error("lseek failed",errno,__FILE__,__LINE__);
	if (::write(file,nstr,nlen) != len) error("write fail",errno,__FILE__,__LINE__);
	lock.l_type = F_UNLCK;
	if (fcntl(file,F_SETLK,&lock) < 0) error("fcntl failed",errno,__FILE__,__LINE__);
	parse.cleanup(nstr);
	return 1;
}

// write string to end of file
void Read::write(const char *str)
{
	int len = strlen(str);
	if (lseek(file,fpos,SEEK_SET) < 0) error("lseek failed",errno,__FILE__,__LINE__);
	int num = ::write(file,str,len);
	if (num != len) error("write failed",errno,__FILE__,__LINE__);
}

// wait for readlock or return nonzero if interrupted by signal
int Read::getrdlck()
{
	struct flock lock;
	lock.l_start = fpos; lock.l_len = 1; lock.l_type = F_RDLCK; lock.l_whence = SEEK_SET;
	int num = fcntl(file,F_SETLKW,&lock);
	if (num < 0 && errno != EINTR) error("fcntl failed",errno,__FILE__,__LINE__);
	return (num == 0);
}

// release readlock
void Read::unrdlck()
{
	struct flock lock;
	lock.l_start = fpos; lock.l_len = 1; lock.l_type = F_UNLCK; lock.l_whence = SEEK_SET;
	if (fcntl(file,F_SETLK,&lock) < 0) error("fcntl failed",errno,__FILE__,__LINE__);
}
