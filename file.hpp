/*
*    file.hpp thread for reading and appending file
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

#ifndef FILE_H
#define FILE_H

#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <map>
#include <deque>

#define BUFFER_SIZE 100
#define FILE_LENGTH 1000
#define INFINITE_LENGTH 1000000000ull

struct Pid
{
	pid_t pid;
	time_t sec;
};

struct Header {
	off_t loc;
	size_t len; 
	int mod;
	  // mod 0 in fifo causes append of len to given
	  // mod 1 in fifo causes write of len to given at loc
	  // mod 2 in fifo qualified by pid causes read
	  //  of len or less from given at loc
	  // mod 3 in fifo causes terminate
	  // mod 0 or 1 in pipe causes postpone or return of len
	  // mod 2 in pipe causes return of len
	  //  or finish initialize if len is 0
	  // mod 3 in pipe is error
	struct Pid pid;
};

extern "C" void *fileThread(void *ptr);

class File
{
public:
	File(const char *n);
	virtual ~File();
	// read and identify are called by reader thread
	// append and update are called by writer thread
	// records appended or updated are in general read
	// all functions return -1 and errno on system error
	int read(char *buf, int len); // called by Read Thread
	  // initially reads to eof
	  // then waits for appends or updates
	  // returns number of bytes read before contiguity
	int identify(int id, int len, int off); // called by Read Thread
	  // assigns given identifier to record of given length at given offset back
	  // returns -2 if prior reads were not contiguous back to given offset
	  // returns given length
	int append(const char *buf, int len); // called by Write Thread
	  // appends new record
	  // returns given length
	int update(const char *buf, int len, int id, char def); // called by Write Thread
	  // updates identified record
	  // truncates or fills with default to match identified length
	  // falls back to append if id is unassigned
	  // returns truncated filled or given length
private:
	const char *name;
	int given; // read and written by File::run
	  // has only data
	int temp; // read and written by File::run
	  // has only headers
	int fifo[2]; // read by File::run; written by File::read File::append File::update
	  // has headers, and data if header.mod == 0
	int pipe[2]; // read by File::read; written by File::run
	  // has headers, and data regardless of header.mod
	pthread_t thread;
	int running; // read and written by File::run
	int init; // read and written by File::read
	size_t todo; // used by File::read
	off_t done; // used by File::read
	pthread_mutex_t mutex; // used by File::identify and File::update
    std::map<int,Header> ident; // used by File::identify and File::update
    std::deque<char*> pend; // used by File::read
    std::deque<size_t> size; // used by File::read
	Pid pid;
	friend void *fileThread(void *ptr);
	void run();
	void transfer(int src, int dst, int lck, int typ, struct Header &hdr);
	void youngest();
};

#endif
