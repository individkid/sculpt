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

extern "C" void *threadFunc(void *thread);

class Thread
{
private:
	pthread_t thread;
	bool isMain;
public:
	Thread(bool m = false) : isMain(m)
	{
		if (!isMain && pthread_create(&thread,NULL,threadFunc,(void *)this)) {std::cerr << "cannot create thread" << std::endl; exit(-1);}
	}
	virtual void run() = 0;
	virtual void wake() = 0;
	void wait()
	{
		if (isMain) run();
		else if (pthread_join(thread,NULL) != 0) {std::cerr << "cannot join thread" << std::endl; exit(-1);}
	}
};

template <class T>
void insert(T *&list, T *ptr)
{
	for (T *i = list; i; i = i->next)
	if (i == ptr) {std::cerr << "insert duplicate ptr" << std::endl; exit(-1);}
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
	std::cerr << "remove missing ptr" << std::endl; exit(-1);
}
template <class T>
void enque(T *&head, T *&tail, T *ptr)
{
	if ((head == 0) != (tail == 0)) {std::cerr << "enque invalid queue" << std::endl; exit(-1);}
	ptr->next = 0;
	if (head == 0) {head = tail = ptr; return;}
	if (tail->next != 0) {std::cerr << "enque invalid queue" << std::endl; exit(-1);}
	tail->next = ptr; tail = ptr;
}
template <class T>
T *deque(T *&head, T *&tail)
{
	if ((head == 0) != (tail == 0)) {std::cerr << "deque invalid queue" << std::endl; exit(-1);}
	if (head == 0) {std::cerr << "deque empty queue" << std::endl; exit(-1);}
	if (tail->next != 0) {std::cerr << "deque invalid queue" << std::endl; exit(-1);}
	T *rslt = head; head = head->next;
	if (head == 0) tail == 0;
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
		if (pthread_mutex_init(&mutex,NULL) != 0) {std::cerr << "message invalid mutex" << std::endl; exit(-1);}
		if (pthread_cond_init(&cond,NULL) != 0) {std::cerr << "message invalid cond" << std::endl; exit(-1);}
	}
	Message(Thread *ptr) : thread(ptr), head(0), tail(0), pool(0), wait(0)
	{
		if (pthread_mutex_init(&mutex,NULL) != 0) {std::cerr << "message invalid mutex" << std::endl; exit(-1);}
		if (pthread_cond_init(&cond,NULL) != 0) {std::cerr << "message invalid cond" << std::endl; exit(-1);}
	}
	void put(T val)
	{
		Next<T> *ptr;
		if (pool) {ptr = pool; remove(pool,ptr);}
		else ptr = new Next<T>();
		ptr->box = val;
		if (pthread_mutex_lock(&mutex) != 0) {std::cerr << "mutex invalid lock" << std::endl; exit(-1);}		
		while (head && wait) 
		if (pthread_cond_wait(&cond,&mutex) != 0) {std::cerr << "cond invalid wait" << std::endl; exit(-1);}
		enque(head,tail,ptr);
		if (thread) thread->wake();
		if (pthread_mutex_unlock(&mutex) != 0) {std::cerr << "mutex invalid unlock" << std::endl; exit(-1);}
	}
	int get(T &val)
	{
		if (!head) return 0;
		wait = 1;
		if (pthread_mutex_lock(&mutex) != 0) {std::cerr << "mutex invalid lock" << std::endl; exit(-1);}		
		Next<T> *ptr = deque(head,tail);
		val = ptr->box;
		insert(pool,ptr);
		wait = 0;
		if (pthread_cond_signal(&cond) != 0) {std::cerr << "cond invalid signal" << std::endl; exit(-1);}
		if (pthread_mutex_unlock(&mutex) != 0) {std::cerr << "mutex invalid unlock" << std::endl; exit(-1);}
		return 1;
	}
};

#endif
