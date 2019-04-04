/*
*    message.cpp c function for starting threads
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

#include <signal.h>
#include <sys/select.h>

#include "message.hpp"

char *concat(Power<char> &pool, const char *left, const char *right)
{
	char *str = pool.get(strlen(left)+strlen(right)+1);
	return strcat(strcpy(str,left),right);
}

char *concat(Power<char> &pool, char *left, const char *right)
{
	char *str = pool.get(strlen(left)+strlen(right)+1);
	strcat(strcpy(str,left),right);
	pool.put(strlen(left)+1,left);
	return str;
}

char *concat(Power<char> &pool, const char *left, char *right)
{
	char *str = pool.get(strlen(left)+strlen(right)+1);
	strcat(strcpy(str,left),right);
	pool.put(strlen(right)+1,right);
	return str;
}

char *concat(Power<char> &pool, char *left, char *right)
{
	char *str = pool.get(strlen(left)+strlen(right)+1);
	strcat(strcpy(str,left),right);
	pool.put(strlen(left)+1,left);
	pool.put(strlen(right)+1,right);
	return str;
}

char *concat(Power<char> &pool, const char *left, char right)
{
	char *str = pool.get(strlen(left)+1+1);
	strcpy(str,left);
	str[strlen(left)] = right;
	str[strlen(left)+1] = 0;
	return str;
}

char *concat(Power<char> &pool, char *left, char right)
{
	char *str = pool.get(strlen(left)+1+1);
	strcpy(str,left);
	str[strlen(left)] = right;
	str[strlen(left)+1] = 0;
	pool.put(strlen(left)+1,left);
	return str;
}

char *concat(Power<char> &pool, char left, const char *right)
{
	char *str = pool.get(1+strlen(right)+1);
	str[0] = left;
	str[1] = 0;
	strcat(str,right);
	return str;
}

char *concat(Power<char> &pool, char left, char *right)
{
	char *str = pool.get(1+strlen(right)+1);
	str[0] = left;
	str[1] = 0;
	strcat(str,right);
	pool.put(strlen(right)+1,right);
	return str;
}

char *prefix(Power<char> &pool, const char *str, int len)
{
	char *res; const char *src; int num;
	if (len > 0) {num = len; src = str;}
	else {num = -len; src = str+strlen(str)+len;}
	res = strncpy(pool.get(num+1),str,num); res[num] = 0;
	return res;
}

char *prefix(Power<char> &pool, char *str, int len)
{
	char *res; const char *src; int num;
	if (len > 0) {num = len; src = str;}
	else {num = -len; src = str+strlen(str)+len;}
	res = strncpy(pool.get(num+1),str,num); res[num] = 0;
	pool.put(strlen(str)+1,str);
	return res;
}

char *postfix(Power<char> &pool, const char *str, int len)
{
	char *res; const char *src; int num;
	if (len > 0) {num = strlen(str)-len; src = str+len;}
	else {num = strlen(str)+len; src = str;}
	res = strncpy(pool.get(num+1),str,num); res[num] = 0;
	return res;
}

char *postfix(Power<char> &pool, char *str, int len)
{
	char *res; const char *src; int num;
	if (len > 0) {num = strlen(str)-len; src = str+len;}
	else {num = strlen(str)+len; src = str;}
	res = strncpy(pool.get(num+1),str,num); res[num] = 0;
	pool.put(strlen(str)+1,str);
	return res;
}

char *setup(Power<char> &pool, const char *str)
{
	char *res = strcpy(pool.get(strlen(str)+1),str);
	return res;
}

char *setup(Power<char> &pool, char *str)
{
	char *res = strcpy(pool.get(strlen(str)+1),str);
	pool.put(strlen(str)+1,str);
	return res;
}

char *setup(Power<char> &pool, char chr)
{
	char *res = pool.get(1+1); res[0] = chr; res[1] = 0;
	return res;
}

const char *cleanup(Power<char> &pool, char *str)
{
	pool.put(strlen(str)+1,str);
	return str;
}

extern "C" void *threadFunc(void *arg)
{
	Thread *thread = (Thread *)arg;
	thread->run();
	return 0;
}

extern "C" void signalFunc(int sig)
{
}

Thread::Thread(int m) : isMain(m), isDone(0)
{
}

void Thread::fork()
{
	if (isMain) {sigset_t sigs = {0}; sigaddset(&sigs, SIGUSR1);
	if (sigprocmask(SIG_BLOCK,&sigs,0) < 0) error("cannot set mask",errno,__FILE__,__LINE__);}
	else if (pthread_create(&thread,NULL,threadFunc,(void *)this)) error("cannot create thread",errno,__FILE__,__LINE__);
}

void Thread::run()
{
    struct sigaction sigact; sigemptyset(&sigact.sa_mask); sigact.sa_handler = signalFunc;
    if (sigaction(SIGUSR1, &sigact, 0) < 0) error("sigaction faile",errno,__FILE__,__LINE__);
	init(); while (!isDone) {call(); wait();} done();
}

void Thread::wait()
{
	sigset_t unblock;
	if (pthread_sigmask(SIG_SETMASK,0,&unblock)) error ("cannot get mask",errno,__FILE__,__LINE__);
	sigdelset(&unblock, SIGUSR1);
   	if (pselect(0, 0, 0, 0, 0, &unblock) < 0 && errno != EINTR) error("pselect",errno,__FILE__,__LINE__);
}

void Thread::wake()
{
	if (pthread_kill(thread,SIGUSR1) != 0) error("cannot kill thread",errno,__FILE__,__LINE__);
}

void Thread::kill()
{
	isDone = 1; wake(); if (!isMain) {
	if (pthread_join(thread,NULL) != 0) error("cannot join thread",errno,__FILE__,__LINE__);}
}
