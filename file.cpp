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
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>

template <class T> void error(const char *str, T wrt, const char *file, int line)
{
	std::cerr << "error:" << str << " wrt:" << wrt << " file:" << file << " line:" << line << std::endl;
	exit(-1);
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
	if (pthread_mutex_init(&mutex, 0) < 0) error("cannot init",errno,__FILE__,__LINE__);
	youngest();
	if ((progress = lseek(temp,0,SEEK_END)) < 0) error("cannot seek",errno,__FILE__,__LINE__);
	location = 0; offset = 0; length = 0; todo = 0;
	running = 1; initialize = 1;
	// TODO initialize pid
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
	// TODO send header.mod = 2 on fifo[1]
	if (pthread_join(thread, 0) < 0) error("cannot join",errno,__FILE__,__LINE__);
	if (pthread_mutex_destroy(&mutex) < 0) error("cannot destroy",errno,__FILE__,__LINE__);
}

int File::read(char *buf, int max)
{
	// TODO if initialize, and header.mod from pipe is 0, fold into buffer, ignore, or send, depending on header.loc and location
	if (todo == 0 && length == 0) {
		Header header;
		if (initialize) {
			header.pos = SEEK_SET; header.loc = location+offset; header.len = BUFFER_SIZE-length; header.mod = 1;
			if (write(fifo[1],&header,sizeof(header)) != sizeof(header)) error("write failed",errno,__FILE__,__LINE__);}
		if (::read(pipe[0],&header,sizeof(header)) != sizeof(header)) error("read failed",errno,__FILE__,__LINE__);
		if (header.len == 0) initialize = 0;
		if (header.loc != location+offset) {location = header.loc; todo = header.len; offset = 0;}}
	int ready = todo;
	if (ready > BUFFER_SIZE-length) ready = BUFFER_SIZE-length;
	if (ready > 0) {
		if (::read(pipe[0],buffer+length,ready) != ready) error("read failed",errno,__FILE__,__LINE__);
		length += ready; todo -= ready;}
	if (max > length) max = length;
	for (int i = 0; i < max; i++) buf[i] = buffer[i];
	for (int i = 0; i+max < length; i++) buffer[i] = buffer[i+max];
	length -= max; offset += max;
	return max;
}

int File::identify(int id, int len, int off)
{
	return -1;
}

int File::append(const char *buf, int len)
{
	return -1;
}

int File::update(const char *buf, int len, int id, char def)
{
	return -1;
}

void File::run()
{
	//while running
	// abandon and reopen temp if too long
	// try write lock temp
	//  read header from fifo[0]
	//  append header to temp
	//  if len is nonzero
	//   write lock given
	//   read fifo[0]
	//   write given
	//   unlock given
	//  unlock temp
	// or wait read lock temp
	//  read header from temp
	//  unlock temp
	// read lock given
	// read given
	// write pipe[1]
	// unlock given
	while (running) {
		struct Header header; header.mod = 3;
		struct flock lock;
		off_t templen;
		if ((templen = lseek(temp,0,SEEK_END)) < 0) error("cannot seek",errno,__FILE__,__LINE__);
		lock.l_start = progress; lock.l_len = INFINITE_LENGTH; lock.l_type = F_WRLCK; lock.l_whence = SEEK_SET;
		int val = -1;
		if (progress == templen) {
			val = fcntl(temp,F_SETLK,&lock);
			if (val == -1 && errno != EAGAIN) error("cannot fcntl",errno,__FILE__,__LINE__);}
		if (val != -1) {
			if (progress < FILE_LENGTH) {
				if ((templen = lseek(temp,0,SEEK_END)) < 0) error("cannot seek",errno,__FILE__,__LINE__);
				if (progress == templen) {
					if (::read(fifo[0],&header,sizeof(header)) < 0) error("cannot read",errno,__FILE__,__LINE__);
					if (header.mod == 0) transfer(fifo[0],given,given,F_WRLCK,header);
					if (write(temp,&header,sizeof(header)) < 0) error("cannot write",errno,__FILE__,__LINE__);
					progress += sizeof(header);}
				lock.l_start = progress; lock.l_len = INFINITE_LENGTH; lock.l_type = F_UNLCK; lock.l_whence = SEEK_SET;
				if (fcntl(temp,F_SETLK,&lock) < 0) error("cannot fcntl",errno,__FILE__,__LINE__);}
			else {
				if (close(temp) < 0) error("cannot close",errno,__FILE__,__LINE__);
				youngest(); progress = 0;}}
		else {
			lock.l_start = 0; lock.l_len = 1; lock.l_type = F_RDLCK; lock.l_whence = SEEK_END;
			if (fcntl(temp,F_SETLKW,&lock) < 0) error("cannot fcntl",errno,__FILE__,__LINE__);
			if (::read(temp,&header,sizeof(header)) < 0) error("cannot read",errno,__FILE__,__LINE__);
			lock.l_start = 0; lock.l_len = 1; lock.l_type = F_UNLCK; lock.l_whence = SEEK_END;
			if (fcntl(temp,F_SETLK,&lock) < 0) error("cannot fcntl",errno,__FILE__,__LINE__);}
		if (header.mod == 0 || (header.mod == 1 && header.pid.pid == pid.pid && header.pid.sec == pid.sec)) {
			if (write(pipe[1],&header,sizeof(header)) != sizeof(header)) error("cannot write",errno,__FILE__,__LINE__);
			transfer(given,pipe[1],given,F_RDLCK,header);}
		if (header.mod == 2 && header.pid.pid == pid.pid && header.pid.sec == pid.sec) running = 0;}
}

void File::transfer(int src, int dst, int lck, int typ, struct Header &hdr)
{
	char buffer[BUFFER_SIZE];
	struct flock lock;
	lock.l_start = hdr.loc; lock.l_len = hdr.len; lock.l_type = typ; lock.l_whence = hdr.pos;
	if (fcntl(lck,F_SETLKW,&lock) < 0) error("cannot fcntl",errno,__FILE__,__LINE__);
	hdr.loc = lseek(lck,hdr.loc,hdr.pos); hdr.pos = SEEK_SET;
	if (hdr.loc < 0) error("cannot seek",errno,__FILE__,__LINE__);
	size_t len = hdr.len;
	while (len) {
		int min = len;
		if (min > BUFFER_SIZE) min = BUFFER_SIZE;
		if (::read(src,buffer,min) != min) error("cannot read",errno,__FILE__,__LINE__);
		if (write(dst,buffer,min) != min) error("cannot write",errno,__FILE__,__LINE__);
		len -= min;}
	lock.l_start = hdr.loc; lock.l_len = hdr.len; lock.l_type = F_UNLCK; lock.l_whence = hdr.pos;
	if (fcntl(lck,F_SETLK,&lock) < 0) error("cannot fcntl",errno,__FILE__,__LINE__);
}

void File::youngest()
{
	// TODO glob for youngest of .temp*.name or create new one, and open it in temp
}
