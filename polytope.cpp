/*
*    polytope.cpp thread for sample and classify
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

#include <unistd.h>
#include <sys/select.h>
#include <signal.h>
#include <libgen.h>
#include "polytope.hpp"
#include "read.hpp"
#include "write.hpp"
#include "script.hpp"
#include "window.hpp"
#include "stream.hpp"

static Stream stream(__FILE__,__LINE__);

Polytope::Polytope(int n, const char *path) : Thread(), nfile(n), iss(0),
	rsp2read(new Message<Data>*[n]), req2write(new Message<Data>*[n]),
	read2req(this,"Read->Data->Polytope"), write2rsp(this,"Polytope<-Data<-Write"),
	script2req(this,"Script->Data->Polytope"), window2rsp(this,"Polytope<-Data<-Window"),
	window2req(this,"Window->Data->Polytope")
{
	if (pipe(p2t) < 0) error("pipe open failed",errno,__FILE__,__LINE__);
	if (pipe(t2p) < 0) error("pipe open failed",errno,__FILE__,__LINE__);
	pid_t pid = fork();
	if (pid < 0) error("cannot fork process",errno,__FILE__,__LINE__);
	if (pid == 0) {
	if (close(p2t[0]) < 0) error("close failed",errno,__FILE__,__LINE__); p2t[0] = -1;
	if (close(t2p[1]) < 0) error("close failed",errno,__FILE__,__LINE__); t2p[1] = -1;
	char *argv[4];
	char buffer[strlen(path)+1];
	if (asprintf(&argv[0],"%s/haskell",dirname_r(path,buffer)) < 0) error("asprintf failed",errno,__FILE__,__LINE__);
	if (asprintf(&argv[1],"%d",t2p[0]) < 0) error("asprintf failed",errno,__FILE__,__LINE__);
	if (asprintf(&argv[2],"%d",p2t[1]) < 0) error("asprintf failed",errno,__FILE__,__LINE__);
	argv[3] = 0;
	if (execv(argv[0],argv) < 0) error("execv failed",errno,__FILE__,__LINE__);
	error("execute sculpt-hs failed",errno,__FILE__,__LINE__);}
	nfd = (p2t[0]>t2p[1]?p2t[0]:t2p[0])+1;
}

Polytope::~Polytope()
{
	Command *command; Data *data;
	while (write2rsp.get(data)) stream.Pools::put(data);
	while (window2rsp.get(command)) stream.Pools::put(command);
}

void Polytope::connect(int i, Read *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
	rsp2read[i] = &ptr->polytope2rsp;
}

void Polytope::connect(int i, Write *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
	req2write[i] = &ptr->polytope2req;
}

void Polytope::connect(Script *ptr)
{
	rsp2script = &ptr->polytope2rsp;
}

void Polytope::connect(Window *ptr)
{
	rsp2window = &ptr->polytope2rsp;
	req2window = &ptr->polytope2req;
}

void Polytope::wait()
{
	sigset_t unblock; int fd;
	fd_set readfds; FD_SET(p2t[0],&readfds);
	fd_set errorfds; FD_SET(p2t[0],&errorfds);
	if (pthread_sigmask(SIG_SETMASK,0,&unblock)) error ("cannot get mask",errno,__FILE__,__LINE__);
	sigdelset(&unblock, SIGUSR1);
   	if (pselect(nfd, &readfds, 0, &errorfds, 0, &unblock) < 0 && errno != EINTR) error("pselect",errno,__FILE__,__LINE__);
   	iss = (FD_ISSET(p2t[0],&readfds) || FD_ISSET(p2t[0],&errorfds));
}

void Polytope::init()
{
	for (int i = 0; i < nfile; i++) if (rsp2read[i] == 0) error("unconnected rsp2read",i,__FILE__,__LINE__);
	for (int i = 0; i < nfile; i++) if (req2write[i] == 0) error("unconnected req2write",i,__FILE__,__LINE__);
	if (rsp2script == 0) error("unconnected rsp2script",0,__FILE__,__LINE__);
	if (rsp2window == 0) error("unconnected rsp2window",0,__FILE__,__LINE__);
	if (req2window == 0) error("unconnected req2window",0,__FILE__,__LINE__);
}

void Polytope::call()
{
	Data *data; Command *command;
    data = 0; while (read2req.get(data)) {
    printf("polytope:%s",data->text); rsp2read[data->file]->put(data);}
    return;
	if (iss) {iss = 0;
	Opcode opcode = stream.get(p2t[0],data,command);
	switch (opcode) {
	case (ReadOp): rsp2read[data->file]->put(data); break;
	case (WriteOp): rsp2read[data->file]->put(data); break;
	case (ScriptOp): rsp2script->put(data); break;
	case (WindowOp): rsp2window->put(data); break;
	case (CommandOp): req2window->put(command); break;
	default: error("invalid opcode",opcode,__FILE__,__LINE__);}}
	while (read2req.get(data)) stream.put(t2p[1],ReadOp,data);
	while (write2rsp.get(data)) stream.put(t2p[1],WriteOp,data);
	while (script2req.get(data)) stream.put(t2p[1],ScriptOp,data);
	while (window2rsp.get(command)) stream.put(t2p[1],CommandOp,command);
	while (window2req.get(data)) stream.put(t2p[1],WindowOp,data);
}

void Polytope::done()
{
	Data *data;
	while (read2req.get(data)) rsp2read[data->file]->put(data);
	while (script2req.get(data)) rsp2script->put(data);
	while (window2req.get(data)) rsp2window->put(data);
}
