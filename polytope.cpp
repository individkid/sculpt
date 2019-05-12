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
#include <stdio.h>
#include "polytope.hpp"
#include "read.hpp"
#include "write.hpp"
#include "script.hpp"
#include "window.hpp"

static Pool<Data> datas(__FILE__,__LINE__);
static Pool<Command> commands(__FILE__,__LINE__);

void Polytope::close(int hs2close, int close2hs)
{
	if (::close(hs2read[hs2close]) != 0) error("cannot close pipe",errno,__FILE__,__LINE__);
	if (::close(read2hs[close2hs]) != 0) error("cannot close pipe",errno,__FILE__,__LINE__);
	if (::close(hs2data[hs2close]) != 0) error("cannot close pipe",errno,__FILE__,__LINE__);
	if (::close(data2hs[close2hs]) != 0) error("cannot close pipe",errno,__FILE__,__LINE__);
	if (::close(hs2script[hs2close]) != 0) error("cannot close pipe",errno,__FILE__,__LINE__);
	if (::close(script2hs[close2hs]) != 0) error("cannot close pipe",errno,__FILE__,__LINE__);
	if (::close(hs2window[hs2close]) != 0) error("cannot close pipe",errno,__FILE__,__LINE__);
	if (::close(window2hs[close2hs]) != 0) error("cannot close pipe",errno,__FILE__,__LINE__);
	if (::close(hs2command[hs2close]) != 0) error("cannot close pipe",errno,__FILE__,__LINE__);
	if (::close(command2hs[close2hs]) != 0) error("cannot close pipe",errno,__FILE__,__LINE__);
}

Polytope::Polytope(int n, const char *path) : Thread(), nfile(n), rsp2read(new Message<Data*>*[n]),
	req2write(new Message<Data*>*[n]), read2req(this,"Read->Data->Polytope"),
	write2rsp(this,"Polytope<-Data<-Write"), script2req(this,"Script->Data->Polytope"),
	window2rsp(this,"Polytope<-Data<-Window"), window2req(this,"Window->Data->Polytope")
{
	if (pipe(hs2read) != 0) error("cannot open pipe",errno,__FILE__,__LINE__);
	if (pipe(read2hs) != 0) error("cannot open pipe",errno,__FILE__,__LINE__);
	if (pipe(hs2data) != 0) error("cannot open pipe",errno,__FILE__,__LINE__);
	if (pipe(data2hs) != 0) error("cannot open pipe",errno,__FILE__,__LINE__);
	if (pipe(hs2script) != 0) error("cannot open pipe",errno,__FILE__,__LINE__);
	if (pipe(script2hs) != 0) error("cannot open pipe",errno,__FILE__,__LINE__);
	if (pipe(hs2window) != 0) error("cannot open pipe",errno,__FILE__,__LINE__);
	if (pipe(window2hs) != 0) error("cannot open pipe",errno,__FILE__,__LINE__);
	if (pipe(hs2command) != 0) error("cannot open pipe",errno,__FILE__,__LINE__);
	if (pipe(command2hs) != 0) error("cannot open pipe",errno,__FILE__,__LINE__);
	pid_t pid = fork();
	if (pid < 0) error("cannot fork process",errno,__FILE__,__LINE__);
	int hs2close = (pid == 0 ? 0 : 1); // haskell only writes to hs2*
	int close2hs = (pid == 0 ? 1 : 0); // haskell only reads from *2hs
	close(hs2close,close2hs);
	hs2open = (pid == 0 ? 1 : 0);
	open2hs = (pid == 0 ? 0 : 1);
	if (pid == 0) {
	char *argv[12];
	if (asprintf(&argv[0],"%s-hs",path) < 0) error("asprintf failed",errno,__FILE__,__LINE__);
	if (asprintf(&argv[1],"%d",hs2read[hs2open]) < 0) error("asprintf failed",errno,__FILE__,__LINE__);
	if (asprintf(&argv[2],"%d",read2hs[open2hs]) < 0) error("asprintf failed",errno,__FILE__,__LINE__);
	if (asprintf(&argv[3],"%d",hs2data[hs2open]) < 0) error("asprintf failed",errno,__FILE__,__LINE__);
	if (asprintf(&argv[4],"%d",data2hs[open2hs]) < 0) error("asprintf failed",errno,__FILE__,__LINE__);
	if (asprintf(&argv[5],"%d",hs2script[hs2open]) < 0) error("asprintf failed",errno,__FILE__,__LINE__);
	if (asprintf(&argv[6],"%d",script2hs[open2hs]) < 0) error("asprintf failed",errno,__FILE__,__LINE__);
	if (asprintf(&argv[7],"%d",hs2window[hs2open]) < 0) error("asprintf failed",errno,__FILE__,__LINE__);
	if (asprintf(&argv[8],"%d",window2hs[open2hs]) < 0) error("asprintf failed",errno,__FILE__,__LINE__);
	if (asprintf(&argv[9],"%d",hs2command[hs2open]) < 0) error("asprintf failed",errno,__FILE__,__LINE__);
	if (asprintf(&argv[10],"%d",command2hs[open2hs]) < 0) error("asprintf failed",errno,__FILE__,__LINE__);
	argv[11] = 0;
	if (execv(argv[0],argv) < 0) error("execv failed",errno,__FILE__,__LINE__);
	error("execute sculpt-hs failed",errno,__FILE__,__LINE__);}
}

Polytope::~Polytope()
{
	Command *command; Data *data;
	while (write2rsp.get(data)) datas.put(data);
	while (window2rsp.get(command)) commands.put(command);
	close(hs2open,open2hs);
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
    Data *data = 0; while (read2req.get(data)) {
    printf("polytope:%s",data->text); rsp2read[data->file]->put(data);}
}

void Polytope::done()
{
	Data *data;
	while (read2req.get(data)) rsp2read[data->file]->put(data);
	while (script2req.get(data)) rsp2script->put(data);
	while (window2req.get(data)) rsp2window->put(data);
}
