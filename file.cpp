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

#define  MAX_TEMP_LENGTH 1000

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
	if ((temp = youngest(name)) < 0) error("cannot open",errno,__FILE__,__LINE__);
	if (pthread_mutex_init(&mutex, 0) < 0) error("cannot init",errno,__FILE__,__LINE__);
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
	running = 0;
	if (pthread_join(thread, 0) < 0) error("cannot join",errno,__FILE__,__LINE__);
	if (pthread_mutex_destroy(&mutex) < 0) error("cannot destroy",errno,__FILE__,__LINE__);
}

int File::read(char *buf, int max)
{
	if (todo == 0 && length == 0) {
	  if (initialize) {
	    // send location/zero/pid on fifo[1]
	  }
	  Header header;
	  if (::read(pipe[0],&header,sizeof(header)) != sizeof(header))
	    error("read failed",errno,__FILE__,__LINE__);
	  if (header.len == 0 && initialize) initialize = 0;
	  if (header.len == 0 && !initialize) running = 0;
	  if (header.loc != location+offset) {
	    location = header.loc; todo = header.len; offset = 0;}}
	int ready = todo;
	if (ready > BUFFER_SIZE-length) ready = BUFFER_SIZE-length;
	if (::read(pipe[0],buffer+length,ready) != ready)
	  error("read failed",errno,__FILE__,__LINE__);
	length += ready; todo -= ready;
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
	// try write lock temp
	//  read fifo[0]
	//  if len is nonzero
	//   write lock given
	//   write given
	//   unlock given
	//  append temp
	//  unlock temp
	//  abandon and reopen temp if too long
	//  if len is zero and pid is self
	//   read lock given
	//   read given
	//   write pipe[1]
	//   unlock given
	//  write pipe[1]
	// or wait read lock temp
	//  unlock temp
	//  read temp
	//  if len is nonzero
	//   write pipe[1]
	//  if len is zero and pid is self
	//   read lock given
	//   read given
	//   write pipe[1]
	//   unlock given
}

int File::youngest(const char *name)
{
	// TODO glob for youngest of .temp*.name or create new one
	return -1;
}
