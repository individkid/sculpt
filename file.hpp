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
	int read(char *buf, int len);
	  // initially reads to eof
	  // then waits for appends or updates
	  // returns number of bytes read before contiguity
	int identify(int id, int len, int off);
	  // assigns given identifier to record of given length at given offset back
	  // returns -2 if prior reads were not contiguous back to given offset
	  // returns given length
	int append(const char *buf, int len);
	  // appends new record
	  // returns given length
	int update(const char *buf, int len, int id, char def);
	  // updates identified record
	  // truncates or fills with default to match identified length
	  // falls back to append if id is unassigned
	  // returns truncated filled or given length
private:
	const char *name;
	int given, temp, fifo[2], pipe[2];
	pthread_t thread;
	int running;
	// TODO mutex and sparse array for identifiers
	friend void *fileThread(void *ptr);
	void run();
	int youngest(const char *name);
};

#endif
