/*
*    file.cpp thread for reading and appending file
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

#include "file.hpp"

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>

template <class T> void error(const char *str, T wrt, const char *file, int line)
{
	std::cerr << "error:" << str << " wrt:" << wrt << " file:" << file << " line:" << line << std::endl;
	exit(-1);
}

bool operator!=(const Pid &lhs, const Pid &rhs)
{
	return (lhs.pid != rhs.pid || lhs.sec != rhs.sec);
}

extern "C" void *fileThread(void *ptr)
{
	File *file = (File *)ptr;
	file->run();
	return 0;
}

File::File(const char *n) : name(n)
{
	char *pname = new char[strlen(name)+7]; strcpy(pname,".fifo."); strcat(pname,name);
	if (mkfifo(pname,0666) < 0 && errno != EEXIST) error("cannot open",errno,__FILE__,__LINE__);
	if ((fifo[0] = open(pname,O_RDONLY)) < 0) error("cannot open",errno,__FILE__,__LINE__);
	if ((fifo[1] = open(pname,O_WRONLY)) < 0) error("cannot open",errno,__FILE__,__LINE__);
	if (::pipe(pipe) < 0) error("cannot open",errno,__FILE__,__LINE__);
	if ((given = open(name,O_RDWR)) < 0) error("cannot open",errno,__FILE__,__LINE__);
	if ((errno = pthread_mutex_init(&mutex, 0)) != 0) error("cannot init",errno,__FILE__,__LINE__);
	int val; int len; int fd[4]; rr = -1;
	for (int i = 0; i < 3; i++) {
		tempname[i] = new char[strlen(name)+8];
		if (dirname_r(name,tempname[i]) == 0) error("dirname failed",errno,__FILE__,__LINE__);
		strcat(tempname[i],".temp"); len = strlen(tempname[i]);
		tempname[i][len] = '0'+i; tempname[i][len+1] = '.'; tempname[i][len+2] = 0;
		if (basename_r(name,tempname[i]+len+2) == 0) error("basename failed",errno,__FILE__,__LINE__);
		if ((fd[i] = open(tempname[i],O_RDWR)) < 0 && errno != ENOENT) error("open failed",errno,__FILE__,__LINE__);}
	for (int i = 0; i < 3; i++) {
		if (fd[(i+1)%3] < 0 && fd[i] >= 0) {rr = i; break;}}
	if (rr < 0) {rr = 0;
		if ((temp = open(tempname[0],O_RDWR|O_CREAT,0666)) < 0) error("cannot open",errno,__FILE__,__LINE__);}
	else {temp = fd[rr]; for (int i = 0; i < 3; i++) {if (i != rr && fd[i] >= 0) {
		if (close(fd[i]) < 0) error("close failed",errno,__FILE__,__LINE__);}}}
	if ((temppos = lseek(temp,0,SEEK_END)) < 0) error("cannot seek",errno,__FILE__,__LINE__);
	done = 0; todo = 0; init = 1; offset = 0; length = 0;
	pid.pid = getpid(); time(&pid.sec);
	if (pthread_create(&thread, 0, fileThread, this) < 0) error("cannot create",errno,__FILE__,__LINE__);
}

File::~File()
{
	if (close(fifo[0]) < 0) error("close failed",errno,__FILE__,__LINE__);
	if (close(fifo[1]) < 0) error("close failed",errno,__FILE__,__LINE__);
	if (close(pipe[0]) < 0) error("close failed",errno,__FILE__,__LINE__);
	if (close(pipe[1]) < 0) error("close failed",errno,__FILE__,__LINE__);
	if (close(given) < 0) error("close failed",errno,__FILE__,__LINE__);
	if (close(temp) < 0) error("close failed",errno,__FILE__,__LINE__);
	Header header; header.head = FinishHead;
	if (write(fifo[1],&header,sizeof(header)) != sizeof(header)) error("cannot write",errno,__FILE__,__LINE__);
	if (pthread_join(thread, 0) < 0) error("cannot join",errno,__FILE__,__LINE__);
	if ((errno = pthread_mutex_destroy(&mutex)) != 0) error("cannot destroy",errno,__FILE__,__LINE__);
}

int File::ready()
{
	return todo;
}

int File::read(char *buf, int max)
{
	if (init) {
		Header header;
		header.loc = done; header.len = BUFFER_LENGTH; header.head = ReadHead; header.pid = pid;
		if (write(fifo[1],&header,sizeof(header)) != sizeof(header)) error("write failed",errno,__FILE__,__LINE__);
		done += BUFFER_LENGTH;
		if (todo == 0) while (1) {
			if (::read(pipe[0],&header,sizeof(header)) < 0) error("cannot read",errno,__FILE__,__LINE__);
			if (header.head == ReadHead) {todo = header.len; break;}
			size.push_back(header.len);
			char *buf = new char[header.len];
			pend.push_back(buf);
			base.push_back(header.loc);
			if (::read(pipe[0],buf,header.len) != header.len) error("cannot read",errno,__FILE__,__LINE__);}
		if (todo == 0) init = 0; else {
			if (todo < max) max = todo;
			todo -= max; length += max;
			if (::read(pipe[0],buf,max) != max) error("cannot read",errno,__FILE__,__LINE__);
			return max;}}
	if (todo == 0 && size.size()) todo = size.front();
	if (size.size()) {
		char *ptr = pend.front();
		size_t len = size.front();
		off_t loc = base.front();
		if (loc+len-todo != offset+length) {offset = loc; length = 0;}
		if (todo < max) max = todo;
		for (int i = 0; i < max; i++) buf[i] = ptr[len-todo+i];
		todo -= max; length += max;
		if (todo == 0) {delete ptr; pend.pop_front(); size.pop_front(); base.pop_front();}
		return max;}
	if (todo == 0) {
		Header header;
		if (::read(pipe[0],&header,sizeof(header)) != sizeof(header)) error("cannot read",errno,__FILE__,__LINE__);
		if (header.loc != offset+length) {offset = header.loc; length = 0;}
		todo = header.len;}
	if (todo < max) max = todo;
	todo -= max; length += max;
	if (::read(pipe[0],buf,max) != max) error("cannot read",errno,__FILE__,__LINE__);
	return max;
}

int File::identify(int id, int len, int off)
{
	if (off > length) return -2;
	if (len > off) return -2;
	if ((errno = pthread_mutex_lock(&mutex)) != 0) error("cannot mutex",errno,__FILE__,__LINE__);
	id2loc[id] = offset+length-off; id2loc[id] = len;
	if ((errno = pthread_mutex_unlock(&mutex)) != 0) error("cannot mutex",errno,__FILE__,__LINE__);
	return 0;
}

int File::append(const char *buf, size_t len)
{
	Header header; header.head = AppendHead; header.len = len;
	if (write(fifo[1],&header,sizeof(header)) != sizeof(header)) error("cannot write",errno,__FILE__,__LINE__);
	if (write(fifo[1],buf,len) != len) error("cannot write",errno,__FILE__,__LINE__);
	return 0;
}

int File::update(const char *buf, size_t len, int id, char def)
{
	Header header; header.head = WriteHead;
	if ((errno = pthread_mutex_lock(&mutex)) != 0) error("cannot mutex",errno,__FILE__,__LINE__);
	header.loc = id2loc[id]; header.len = id2loc[id];
	if ((errno = pthread_mutex_unlock(&mutex)) != 0) error("cannot mutex",errno,__FILE__,__LINE__);
	if (header.len < len) len = header.len;
	if (write(fifo[1],&header,sizeof(header)) != sizeof(header)) error("cannot write",errno,__FILE__,__LINE__);
	if (write(fifo[1],buf,len) != len) error("cannot write",errno,__FILE__,__LINE__);
	while (len++ < header.len) if (write(fifo[1],&def,1) != 1) error("cannot write",errno,__FILE__,__LINE__);
	return 0;
}

void File::run()
{
	while (1) {
	struct Header header;
	struct flock lock;
	int val;
	off_t pos;
	if (temppos >= FILE_LENGTH) {
		if (open(tempname[(rr+2)%3],O_RDWR) >= 0) error("file exists",errno,__FILE__,__LINE__);
		if (close(temp) < 0) error("cannot close",errno,__FILE__,__LINE__);
		if ((temp = open(tempname[(rr+1)%3],O_RDWR|O_CREAT,0666)) < 0) error("cannot open",errno,__FILE__,__LINE__);
		if (unlink(tempname[(rr+0)%3]) < 0 && errno != ENOENT) error("cannot unlink",errno,__FILE__,__LINE__);
		temppos = 0; rr = (rr+1)%3;}
	lock.l_start = 0; lock.l_len = INFINITE_LENGTH; lock.l_type = F_WRLCK; lock.l_whence = SEEK_CUR;
	if ((val = fcntl(temp,F_SETLK,&lock)) < 0 && errno != EAGAIN) error("cannot fcntl",errno,__FILE__,__LINE__);
	if ((pos = lseek(temp,0,SEEK_END)) < 0) error("cannot seek",errno,__FILE__,__LINE__);
	if (pos != temppos) {
		if (lseek(temp,temppos,SEEK_SET) < 0) error("cannot seek",errno,__FILE__,__LINE__);}
	if (val == 0 && pos != temppos) {
		lock.l_start = 0; lock.l_len = INFINITE_LENGTH; lock.l_type = F_UNLCK; lock.l_whence = SEEK_CUR;
		if (fcntl(temp,F_SETLK,&lock) == -1) error("cannot fcntl",errno,__FILE__,__LINE__);}
	if (val < 0 || pos != temppos) {
		lock.l_start = 0; lock.l_len = 1; lock.l_type = F_RDLCK; lock.l_whence = SEEK_CUR;
		if (fcntl(temp,F_SETLKW,&lock) < 0) error("cannot fcntl",errno,__FILE__,__LINE__);
		if (::read(temp,&header,sizeof(header)) != sizeof(header)) error("cannot read",errno,__FILE__,__LINE__);
		lock.l_start = 0; lock.l_len = 1; lock.l_type = F_UNLCK; lock.l_whence = SEEK_CUR;
		if (fcntl(temp,F_SETLK,&lock) < 0) error("cannot fcntl",errno,__FILE__,__LINE__);}
	if (val == 0 && pos == temppos) {
		if (::read(fifo[0],&header,sizeof(header)) != sizeof(header)) error("cannot read",errno,__FILE__,__LINE__);
		if (header.head == AppendHead || header.head == WriteHead) transfer(fifo[0],given,given,F_WRLCK,header);
		if (write(temp,&header,sizeof(header)) != sizeof(header)) error("cannot write",errno,__FILE__,__LINE__);
		lock.l_start = -sizeof(header); lock.l_len = INFINITE_LENGTH; lock.l_type = F_UNLCK; lock.l_whence = SEEK_CUR;
		if (fcntl(temp,F_SETLK,&lock) == -1) error("cannot fcntl",errno,__FILE__,__LINE__);}
	temppos += sizeof(header);
	if (header.head == ReadHead && header.pid != pid) continue;
	if (header.head == FinishHead && header.pid != pid) continue;
	if (header.head == FinishHead) break;
	if (header.head == ReadHead) {
		off_t givenlen;
		if ((givenlen = lseek(given,0,SEEK_END)) < 0) error("cannot seek",errno,__FILE__,__LINE__);
		if (header.loc + header.len > givenlen) header.len = givenlen - header.loc;}
	if (write(pipe[1],&header,sizeof(header)) != sizeof(header)) error("cannot write",errno,__FILE__,__LINE__);
	transfer(given,pipe[1],given,F_RDLCK,header);}
}

void File::transfer(int src, int dst, int lck, int typ, struct Header &hdr)
{
	char buffer[BUFFER_LENGTH];
	struct flock lock;
	if (hdr.head == AppendHead) lock.l_start = 0; else lock.l_start = hdr.loc;
	lock.l_len = hdr.len; lock.l_type = typ;
	if (hdr.head == AppendHead) lock.l_whence = SEEK_END; else lock.l_whence = SEEK_SET;
	if (fcntl(lck,F_SETLKW,&lock) < 0) error("cannot fcntl",errno,__FILE__,__LINE__);
	if ((lock.l_start = lseek(lck,lock.l_start,lock.l_whence)) < 0) error("cannot seek",errno,__FILE__,__LINE__);
	hdr.head = WriteHead; hdr.loc = lock.l_start; lock.l_whence = SEEK_SET;
	size_t len = lock.l_len;
	while (len > 0) {
		int min = len;
		if (min > BUFFER_LENGTH) min = BUFFER_LENGTH;
		if (::read(src,buffer,min) != min) error("cannot read",errno,__FILE__,__LINE__);
		if (write(dst,buffer,min) != min) error("cannot write",errno,__FILE__,__LINE__);
		len -= min;}
	lock.l_type = F_UNLCK;
	if (fcntl(lck,F_SETLK,&lock) < 0) error("cannot fcntl",errno,__FILE__,__LINE__);
}
