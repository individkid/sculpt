/*
*    system.hpp thread for producing sound
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

#include "message.hpp"
#include "pqueue.h"
#include "portaudio.h"

class Read;
class Script;

extern "C" int systemFunc(
	const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData);

class System : public Thread
{
private:
	Message<Sound> **rsp2sound;
	Message<Data> **rsp2read;
	Message<Data> *req2script;
public:
	Message<Sound> sound2req;
	Message<Data> read2req;
	Message<Data> script2rsp;
public:
	void connect(int i, Read *ptr);
	void connect(Script *ptr);
private:
	virtual void init();
	virtual void call();
	virtual void done();
public:
	System(int n);
	virtual ~System();
private:
	int nfile; int cleanup; int size;
	pqueue_t sample;
	pqueue_t metric;
	pqueue_t update;
	Sound *stodo;
	Data *dtodo;
	float *lwave;
	float *rwave;
	int *lcount;
	int *rcount;
	PaStream *stream;
	PaTime wbeat, rbeat;
	int windex, rindex;
private:
	friend int systemFunc(
		const void *inputBuffer, void *outputBuffer,
	    unsigned long framesPerBuffer,
	    const PaStreamCallbackTimeInfo* timeInfo,
	    PaStreamCallbackFlags statusFlags,
	    void *userData);
	void callback(float *out, PaTime current, int frames);
	void callforth(Equ &left, Equ &right);
	void calladd(float value, float *wave, int *count);
	void callinit(float value, float *wave, int *count);
	double evaluate(Equ &equation);
	void processSounds(Message<Sound> &message);
	void processDatas(Message<Data> &message);
};
