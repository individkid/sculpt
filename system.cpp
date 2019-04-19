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
#include "portaudio.h"

static Pool<Data> datas(__FILE__,__LINE__);
static Power<float> floats(__FILE__,__LINE__);
static Power<char> chars(__FILE__,__LINE__);
static Sparse<int,int> ints(__FILE__,__LINE__);

static int callback(
	const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData)
{
	return 0;
}

void System::processSounds(Message<Sound*> &message)
{
	Sound *sound; while (message.get(sound)) {
		if (!cleanup) {
			if (&message == &sound2req) {
				// TODO add stock to state
			}
		} else {
			if (&message == &sound2req) {
				rsp2sound[sound->file]->put(sound);
			}
		}
	}
}

void System::processDatas(Message<Data*> &message)
{
	Data *data; while (message.get(data)) {
		if (!cleanup) {
			if (&message == &read2req) {
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
		} else {
			if (&message == &read2req) {
				rsp2read[data->file]->put(data);
			}
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
	rsp2sound(new Message<Sound*>*[n]), rsp2read(new Message<Data*>*[n]),
	sound2req(this,"Read->Sound->System"), read2req(this,"Read->Data->System"),
	script2rsp(this,"System<-Data<-Script"), script2req(this,"Script->Data->System")
{
	PaError err = Pa_Initialize();
	if (err != paNoError) error("portaudio init failed",Pa_GetErrorText(err),__FILE__,__LINE__);
}

System::~System()
{
	PaError err = Pa_Terminate();
	if (err != paNoError) error("portaudio term failed",Pa_GetErrorText(err),__FILE__,__LINE__);
	if (!cleanup) error("done not called",0,__FILE__,__LINE__);
	processDatas(script2rsp);
}

void System::connect(int i, Read *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    rsp2sound[i] = &ptr->sound2rsp;
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
	processSounds(sound2req);
	processDatas(read2req);
	processDatas(script2rsp);
	processDatas(script2req);
}

void System::done()
{
	cleanup = 1;
	processSounds(sound2req);
	processDatas(read2req);
	processDatas(script2req);
	// TODO cleanup Sound and Data in timewheel
}
