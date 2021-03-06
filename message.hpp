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

extern "C" {
#include "types.h"
}

#include <set>
#include <map>

enum Identity {
	NameIdent,
	NumberIdent,
	Identities};
struct Unique
{
	enum Identity ident;
	union {int number; const char *name;};
	bool operator()(const Unique &lhs, const Unique &rhs) const
	{if (ident == NameIdent) return (strcmp(lhs.name,rhs.name) < 0);
	return (lhs.number < rhs.number);}
};
struct Range
{
	int base;
	int size;
	bool operator()(const Range &lhs, const Range &rhs) const
	{return (lhs.base+lhs.size <= rhs.base);}
};
struct Holes
{
	std::map<Unique,int,Unique> unique;
	std::set<Range,Range> range;
	int get();
	int get(int num);
	int get(const char *nam);
};

template <class T> class Power;

char *concat(Power<char> &pool, const char *left, const char *right);
char *concat(Power<char> &pool, char *left, const char *right);
char *concat(Power<char> &pool, const char *left, char *right);
char *concat(Power<char> &pool, char *left, char *right);
char *concat(Power<char> &pool, const char *left, char right);
char *concat(Power<char> &pool, char *left, char right);
char *concat(Power<char> &pool, char left, const char *right);
char *concat(Power<char> &pool, char left, char *right);
char *prefix(Power<char> &pool, const char *str, int len);
char *prefix(Power<char> &pool, char *str, int len);
char *postfix(Power<char> &pool, const char *str, int len);
char *postfix(Power<char> &pool, char *str, int len);
char *setup(Power<char> &pool, const char *str);
char *setup(Power<char> &pool, char *str);
char *setup(Power<char> &pool, char chr);
const char *cleanup(Power<char> &pool, char *str);
extern "C" void *threadFunc(void *thread);
extern "C" void signalFunc(int sig);

template <class T> void error(const char *str, T wrt, const char *file, int line)
{
	std::cerr << "error:" << str << " wrt:" << wrt << " file:" << file << " line:" << line << std::endl;
	exit(-1);
}

template <class T> void message(const char *str, T wrt, const char *file, int line)
{
	std::cerr << "message:" << str << " wrt:" << wrt << " file:" << file << " line:" << line << std::endl;
}

class Thread
{
private:
	pthread_t thread;
	int valid;
	int isMain;
	int isDone;
public:
	Thread(int m = 0);
	virtual void exec();
	virtual void wake();
	virtual void kill();
protected:
	virtual void wait();
private:
	void run();
	friend void *threadFunc(void *thread);
	virtual void init() {}
	virtual void call() = 0;
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
void remove(T *&head, T *&tail, T *ptr)
{
	remove(head,ptr);
	if (head == 0) tail = 0;
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

template<class S, class T>
class Pair
{
public:
	S s;
	T t;
};
template<class S, class T>
bool operator==(const Pair<S,T> &l, const Pair<S,T> &r)
{
	return (l.s == r.s && l.t == r.t);
}

template <class T>
class Pool
{
private:
	const char *file;
	int line;
	Next<T*> *full; // box is significant
	Next<T*> *free; // box is overwritable
	int size;
public:
	Pool(const char *f, int l, int s = 1) : file(f), line(l), full(0), free(0), size(s) {}
	~Pool()
	{
		Next<T*> *ptr;
		if (free) {message("nonempty pool",0,file,line); return;}
		for (ptr = full; ptr; ptr = ptr->next)
		if (size == 1) delete ptr->box; else delete[] ptr->box;
	}
	T *get()
	{
		Next<T*> *ptr;
		if (!full) {
		ptr = new Next<T*>();
		ptr->box = (size == 1 ? new T() : new T[size]);
		insert(full,ptr);}
		ptr = full; remove(full,ptr); insert(free,ptr);
		return ptr->box;
	}
	void put(T *val)
	{
		Next<T*> *ptr;
		if (!free) error("empty pool",0,file,line);
		ptr = free; remove(free,ptr); insert(full,ptr);
		ptr->box = val;
	}
};

template <class T>
class Power {
private:
	const char *file;
	int line;
	Pool<T> *pool[10];
public:
	Power(const char *f, int l) : file(f), line(l) {for (int i = 0; i < 10; i++) pool[i] = 0;}
	~Power()
	{
		for (int i = 0; i < 10; i++) if (pool[i]) delete pool[i];
	}
	Pool<T> &operator[](int i)
	{
		if (!pool[i]) pool[i] = new Pool<T>(file,line,1<<i);
		return *pool[i];}
	T *get(int siz)
	{
		int pow = 0; while (siz>(1<<pow)) pow++;
		if (pow >= 10) error("string too size",siz,file,line);
		return (*this)[pow].get();
	}
	void put(int siz, T *ptr)
	{
		int pow = 0; while (siz>(1<<pow)) pow++;
		if (pow >= 10) error("string too size",siz,file,line);
		(*this)[pow].put(ptr);
	}
};

template <class T>
class Message
{
private:
	const char *str;
	Thread *thread;
	T *head;
	T *tail;
	int wait;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
public:
	Message(Thread *ptr) : str(0), thread(ptr), head(0), tail(0), wait(0)
	{
		if (pthread_mutex_init(&mutex,NULL) != 0) error((str?str:"message invalid mutex"),errno,__FILE__,__LINE__);
		if (pthread_cond_init(&cond,NULL) != 0) error((str?str:"message invalid cond"),errno,__FILE__,__LINE__);
	}
	Message(Thread *ptr, const char *s) : str(s), thread(ptr), head(0), tail(0), wait(0)
	{
		if (pthread_mutex_init(&mutex,NULL) != 0) error((str?str:"message invalid mutex"),errno,__FILE__,__LINE__);
		if (pthread_cond_init(&cond,NULL) != 0) error((str?str:"message invalid cond"),errno,__FILE__,__LINE__);
	}
	void put(T *val)
	{
		if ((DEBUG & MESSAGE_DEBUG) && str) std::cout << "put " << str << std::endl;
		if (pthread_mutex_lock(&mutex) != 0) error((str?str:"mutex invalid lock"),errno,__FILE__,__LINE__);		
		while (head && wait) 
		if (pthread_cond_wait(&cond,&mutex) != 0) error((str?str:"cond invalid wait"),errno,__FILE__,__LINE__);
		enque(head,tail,val);
		if (thread) thread->wake();
		if (pthread_mutex_unlock(&mutex) != 0) error((str?str:"mutex invalid unlock"),errno,__FILE__,__LINE__);
	}
	int get(T *&val)
	{
		if (!head) return 0;
		wait = 1;
		if (pthread_mutex_lock(&mutex) != 0) error((str?str:"mutex invalid lock"),errno,__FILE__,__LINE__);		
		val = deque(head,tail);
		wait = 0;
		if (pthread_cond_signal(&cond) != 0) error((str?str:"cond invalid signal"),errno,__FILE__,__LINE__);
		if (pthread_mutex_unlock(&mutex) != 0) error((str?str:"mutex invalid unlock"),errno,__FILE__,__LINE__);
		if ((DEBUG & MESSAGE_DEBUG) && str) std::cout << "get " << str << std::endl;
		return 1;
	}
};

#endif
