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

#include <sys/select.h>

#include "message.hpp"

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
	if (isMain) {sigset_t sigs = {0}; sigaddset(&sigs, SIGUSR1);
	if (sigprocmask(SIG_BLOCK,&sigs,0) < 0) error("cannot set mask",errno,__FILE__,__LINE__);}
	else if (pthread_create(&thread,NULL,threadFunc,(void *)this)) error("cannot create thread",errno,__FILE__,__LINE__);
}

void Thread::run()
{
    struct sigaction sigact; sigemptyset(&sigact.sa_mask); sigact.sa_handler = signalFunc;
    if (sigaction(SIGUSR1, &sigact, 0) < 0) error("sigaction faile",errno,__FILE__,__LINE__);
	init(); while (!isDone) {call();
	sigset_t unblock; if (pthread_sigmask(SIG_SETMASK,0,&unblock)) error ("cannot get mask",errno,__FILE__,__LINE__); sigdelset(&unblock, SIGUSR1);
   	if (pselect(0, 0, 0, 0, 0, &unblock) < 0 && errno != EINTR) error("pselect",errno,__FILE__,__LINE__);} done();
}

void Thread::wake()
{
	if (pthread_kill(thread,SIGUSR1) != 0) error("cannot kill thread",errno,__FILE__,__LINE__);
}

void Thread::kill()
{
	if (isMain) call(); else {isDone = 1; wake();
	if (pthread_join(thread,NULL) != 0) error("cannot join thread",errno,__FILE__,__LINE__);}
}
