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

#include "polytope.hpp"
#include "window.hpp"
#include "read.hpp"
#include "write.hpp"
#include "script.hpp"

static Pool<Data> datas(__FILE__,__LINE__);
static Pool<Command> commands(__FILE__,__LINE__);

Polytope::Polytope(int i) : Thread(),
	read2req(this,"Read->Data->Polytope"), write2rsp(this,"Polytope<-Data<-Write"),
	script2rsp(this,"Polytope<-Data<-Script"), script2req(this,"Script->Data->Polytope"),
	window2rsp(this,"Polytope<-Data<-Window"), window2req(this,"Window->Data->Polytope")
{
}

Polytope::~Polytope()
{
	Command *command; Data *data;
	while (write2rsp.get(data)) datas.put(data);
	while (script2rsp.get(data)) datas.put(data);
	while (window2rsp.get(command)) commands.put(command);
}

void Polytope::connect(Read *ptr)
{
	rsp2read = &ptr->polytope2rsp;
}

void Polytope::connect(Window *ptr)
{
	rsp2window = &ptr->polytope2rsp;
	req2window = &ptr->polytope2req;
}

void Polytope::connect(Write *ptr)
{
	req2write = &ptr->polytope2req;
}

void Polytope::connect(Script *ptr)
{
	req2script = &ptr->polytope2req;
	rsp2script = &ptr->polytope2rsp;
}

void Polytope::init()
{
	if (rsp2read == 0) error("unconnected rsp2read",0,__FILE__,__LINE__);
	if (rsp2window == 0) error("unconnected rsp2window",0,__FILE__,__LINE__);
	if (req2write == 0) error("unconnected req2write",0,__FILE__,__LINE__);
	if (req2window == 0) error("unconnected req2window",0,__FILE__,__LINE__);
	if (req2script == 0) error("unconnected req2script",0,__FILE__,__LINE__);
	if (rsp2script == 0) error("unconnected rsp2script",0,__FILE__,__LINE__);
}

void Polytope::call()
{
    Data *data = 0; while (read2req.get(data)) {
    printf("polytope:%s",data->text); rsp2read->put(data);}
}

void Polytope::done()
{
	Data *data;
	while (read2req.get(data)) rsp2read->put(data);
	while (script2req.get(data)) rsp2script->put(data);
	while (window2req.get(data)) rsp2window->put(data);
}
