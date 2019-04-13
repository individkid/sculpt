/*
*    system.cpp thread for producing sound
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

#include "system.hpp"
#include "read.hpp"
#include "script.hpp"

static Pool<Data> datas(__FILE__,__LINE__);
static Power<float> floats(__FILE__,__LINE__);
static Power<char> chars(__FILE__,__LINE__);
static Sparse<int,int> ints(__FILE__,__LINE__);

void System::processDatas(Message<Data*> &message)
{
	Data *data; while (message.get(data)) {
		if (!cleanup) {
			if (&message == &read2req) {
				if (data->conf == SoundConf) {
					// TODO add stock to state
				}
				if (data->conf == MetricConf) {
					// TODO add macro to state
				}
			}
			if (&message == &script2rsp) {
				// TODO update metric value from script result
			}
			if (&message == &script2req) {
				if (data->conf == SoundConf) {
					// TODO fill arguments with stock values
				}
				if (data->conf == MetricConf) {
					// TODO fill arguments with stock values
				}
			}
		}
		if (&message == &read2req) {
			rsp2read[data->file]->put(data);
		}
		if (&message == &script2rsp) {
			floats.put(ints[data->tagbits],data->argument);
			ints.remove(data->tagbits);
			chars.put(strlen(data->script)+1,data->script);
			datas.put(data);
		}
		if (&message == &script2req) {
			rsp2script->put(data);
		}
	}
}

System::System(int n) : nfile(n), cleanup(0),
	rsp2read(new Message<Data*>*[n]), read2req(this,"Read->Data->System"),
	script2rsp(this,"System<-Data<-Script"), script2req(this,"Script->Data->System")
{
}

System::~System()
{
	if (!cleanup) error("done not called",0,__FILE__,__LINE__);
	processDatas(script2rsp);
}

void System::connect(int i, Read *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    rsp2read[i] = &ptr->system2rsp;
}

void System::connect(Script *ptr)
{
	rsp2script = &ptr->system2rsp;
	req2script = &ptr->system2req;
}

void System::init()
{
	for (int i = 0; i < nfile; i++) if (rsp2read[i] == 0) error("unconnected rsp2read",i,__FILE__,__LINE__);
	if (rsp2script == 0) error("unconnected rsp2script",0,__FILE__,__LINE__);
	if (req2script == 0) error("unconnected req2script",0,__FILE__,__LINE__);
}

void System::call()
{
	processDatas(read2req);
	processDatas(script2rsp);
	processDatas(script2req);
}

void System::done()
{
	cleanup = 1;
	processDatas(read2req);
	processDatas(script2req);
}
