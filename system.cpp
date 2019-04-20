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

extern "C" int systemFunc(
	const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData)
{
	System *self = (System*)userData;
	self->callback((float*)outputBuffer,timeInfo->currentTime,framesPerBuffer);
	return 0;
}

void System::callback(float *out, PaTime time, int frames)
{
	PaTime size = (time-rbeat)/BEAT_PERIOD;
	PaTime ratio = size/frames;
	for (int i = 0; i < frames; i++) {
	int index = (int)(rindex+i*ratio)%WAVE_SIZE;
	*out++ = lwave[index];
	*out++ = rwave[index];}
}

void System::callforth(Equ &left, Equ &right)
{
	PaTime time = Pa_GetStreamTime(stream)+WAVE_SIZE*BEAT_PERIOD/2.0;
	while (wbeat < time) {
	float ltemp = lwave[windex];
	float rtemp = rwave[windex];
	windex = (windex+1)%WAVE_SIZE;
	wbeat += BEAT_PERIOD;
	callinit(ltemp,lwave,lcount);
	callinit(rtemp,rwave,rcount);}
	if (left.denom.count) calladd(evaluate(left),lwave,lcount);
	if (right.denom.count) calladd(evaluate(right),rwave,rcount);
}

void System::calladd(float value, float *wave, int *count)
{
	if (value > 1.0 || value < -1.0) return;
	wave[windex] = wave[windex]*count[windex]+value;
	count[windex] += 1;
	wave[windex] /= count[windex];
}

void System::callinit(float value, float *wave, int *count)
{
	count[windex] = 0;
	wave[windex] = value;
}

double System::evaluate(Equ &equation)
{
	// TODO
	return 0.0;
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
				if (data->conf == TimewheelConf) {
					if (data->start) {
						// TODO map ident to pointer in stodo and dtodo
						PaError err = Pa_StartStream(stream);
						if (err != paNoError) error("start stream error",Pa_GetErrorText(err),__FILE__,__LINE__);
					}
					if (data->stop) {
						PaError err = Pa_StopStream(stream);
						if (err != paNoError) error("stop stream error",Pa_GetErrorText(err),__FILE__,__LINE__);
					}
				}
			}
			if (&message == &script2rsp) {
				// TODO update metric value from script result
			}
			if (&message == &script2req) {
				if (data->conf == ScriptConf) {
					// TODO fill arguments with stock values
				}
			}
		} else {
			if (&message == &read2req) {
				rsp2read[data->file]->put(data);
			}
		}
		if (&message == &script2rsp) {
			chars.put(strlen(data->script)+1,data->script);
			datas.put(data);
		}
		if (&message == &script2req) {
			rsp2script->put(data);
		}
	}
}

System::System(int n) : nfile(n), cleanup(0), stodo(0), dtodo(0),
	lwave(new float[WAVE_SIZE]), rwave(new float[WAVE_SIZE]),
	lcount(new int[WAVE_SIZE]), rcount(new int[WAVE_SIZE]),
	wbeat(0.0), rbeat(0.0), windex(0), rindex(0),
	rsp2sound(new Message<Sound*>*[n]), rsp2read(new Message<Data*>*[n]),
	sound2req(this,"Read->Sound->System"), read2req(this,"Read->Data->System"),
	script2rsp(this,"System<-Data<-Script"), script2req(this,"Script->Data->System")
{
	for (int i = 0; i < WAVE_SIZE; i++) lwave[i] = 0.0;
	for (int i = 0; i < WAVE_SIZE; i++) rwave[i] = 0.0;
	for (int i = 0; i < WAVE_SIZE; i++) lcount[i] = 0;
	for (int i = 0; i < WAVE_SIZE; i++) rcount[i] = 0;
	PaError err = Pa_Initialize();
	if (err != paNoError) error("portaudio init failed",Pa_GetErrorText(err),__FILE__,__LINE__);
	err = Pa_OpenDefaultStream(&stream,0,2,paFloat32,SAMPLE_RATE,WAVE_SIZE/4,systemFunc,(void*)this);
	if (err != paNoError) error("portaudo stream failed",Pa_GetErrorText(err),__FILE__,__LINE__);
}

System::~System()
{
	PaError err = Pa_CloseStream(stream);
	if (err != paNoError) error("close stream failed",Pa_GetErrorText(err),__FILE__,__LINE__);
	err = Pa_Terminate();
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
