/*
*    message.hpp template for sending messages between threads
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

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <pthread.h>
#include <string>

extern "C" {
#include "types.h"
}

template <class T> void error(const char *str, T wrt, const char *file, int line)
{
	std::cerr << "error:" << str << " wrt:" << wrt << " file:" << file << " line:" << line << std::endl;
	exit(-1);
}

template <class T> void message(const char *str, T wrt, const char *file, int line)
{
	std::cerr << "message:" << str << " wrt:" << wrt << " file:" << file << " line:" << line << std::endl;
}

template <class T, int N> union Convert
{
	char array[N+1];
	T value;
};

template <class T> std::string convert(T val)
{
	Convert<T,sizeof(T)> convert; convert.value = val;
	std::string str = convert.array; return str;
}

template <class T> void convert(std::string str, T &val)
{
	Convert<T,sizeof(T)> convert; strcpy(convert.array,str.c_str());
	val = convert.value;
}

int cmpstr(std::string str, const char *cstr);
int prestr(std::string str, const char *cstr);

extern "C" void *threadFunc(void *thread);
extern "C" void signalFunc(int sig);

class Thread
{
private:
	pthread_t thread;
	int isMain;
	int isDone;
public:
	Thread(int m = 0);
	virtual void run();
	virtual void init() {}
	virtual void call() = 0;
	virtual void wait();
	virtual void wake();
	virtual void kill();
	virtual void done() {}
};

template <class T>
void insert(T *&list, T *ptr)
{
	for (T *i = list; i; i = i->next)
	if (i == ptr) error("insert duplicate ptr",ptr,__FILE__,__LINE__);
	ptr->next = list; list = ptr;
}
template <class T>
void remove(T *&list, T *ptr)
{
	T *last = 0;
	for (T *i = list; i; i = i->next)
	if (i == ptr && last == 0) {list = ptr->next; return;}
	else if (i == ptr) {last->next = ptr->next; return;}
	else last = i;
	error("remove missing ptr",ptr,__FILE__,__LINE__);
}
template <class T>
void enque(T *&head, T *&tail, T *ptr)
{
	if ((head == 0) != (tail == 0)) error("enque invalid queue",head,__FILE__,__LINE__);
	ptr->next = 0;
	if (head == 0) {head = tail = ptr; return;}
	if (tail->next != 0) error("enque invalid queue",tail->next,__FILE__,__LINE__);
	tail->next = ptr; tail = ptr;
}
template <class T>
T *deque(T *&head, T *&tail)
{
	if ((head == 0) != (tail == 0)) error("deque invalid queue",head,__FILE__,__LINE__);
	if (head == 0) error("deque empty queue",head,__FILE__,__LINE__);
	if (tail->next != 0) error("deque invalid queue",tail->next,__FILE__,__LINE__);
	T *rslt = head; head = head->next;
	if (head == 0) tail = 0;
	return rslt;
}

template <class T>
class Next
{
public:
	Next *next;
	T box;
};

template <class T>
class Pool
{
private:
	Next<T*> *full;
	Next<T*> *free;
public:
	Pool() : full(0), free(0) {}
	T *get()
	{
		if (!full) return new T();
		Next<T*> *ptr = full; remove(full,ptr); insert(free,ptr);
		return ptr->box;
	}
	void put(T *val)
	{
		Next<T*> *ptr;
		if (free) {ptr = free; remove(free,ptr);}
		else ptr = new Next<T*>();
		ptr->box = val;
		insert(full,ptr);
	}
};

template <class T>
class Message
{
private:
	Thread *thread;
	Next<T> *head;
	Next<T> *tail;
	Next<T> *pool;
	int wait;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
public:
	Message() : thread(0), head(0), tail(0), pool(0), wait(0)
	{
		if (pthread_mutex_init(&mutex,NULL) != 0) error("message invalid mutex",errno,__FILE__,__LINE__);
		if (pthread_cond_init(&cond,NULL) != 0) error("message invalid cond",errno,__FILE__,__LINE__);
	}
	Message(Thread *ptr) : thread(ptr), head(0), tail(0), pool(0), wait(0)
	{
		if (pthread_mutex_init(&mutex,NULL) != 0) error("message invalid mutex",errno,__FILE__,__LINE__);
		if (pthread_cond_init(&cond,NULL) != 0) error("message invalid cond",errno,__FILE__,__LINE__);
	}
	void put(T val)
	{
		Next<T> *ptr;
		if (pool) {ptr = pool; remove(pool,ptr);}
		else ptr = new Next<T>();
		ptr->box = val;
		if (pthread_mutex_lock(&mutex) != 0) error("mutex invalid lock",errno,__FILE__,__LINE__);		
		while (head && wait) 
		if (pthread_cond_wait(&cond,&mutex) != 0) error("cond invalid wait",errno,__FILE__,__LINE__);
		enque(head,tail,ptr);
		if (thread) thread->wake();
		if (pthread_mutex_unlock(&mutex) != 0) error("mutex invalid unlock",errno,__FILE__,__LINE__);
	}
	int get(T &val)
	{
		if (!head) return 0;
		wait = 1;
		if (pthread_mutex_lock(&mutex) != 0) error("mutex invalid lock",errno,__FILE__,__LINE__);		
		Next<T> *ptr = deque(head,tail);
		val = ptr->box;
		insert(pool,ptr);
		wait = 0;
		if (pthread_cond_signal(&cond) != 0) error("cond invalid signal",errno,__FILE__,__LINE__);
		if (pthread_mutex_unlock(&mutex) != 0) error("mutex invalid unlock",errno,__FILE__,__LINE__);
		return 1;
	}
};

#endif
