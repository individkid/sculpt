/*
*    script.hpp thread for calculating metrics
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

class Read;
class Window;
class Polytope;
class System;
class Write;

class Script : public Thread
{
private:
	int nfile;
	Message<Data*> **rsp2read;
	Message<Data*> **rsp2polytope;
	Message<Data*> **req2polytope;
	Message<Command*> **req2command;
	Message<Data*> **req2write;
	Message<Data*> *rsp2system;
	Message<Data*> *req2system;
	Message<Command*> *req2window;
	void requestPolytope(/*TODO*/);
	void requestCommand(/*TODO*/);
	void requestWrite(/*TODO*/);
	void requestSystem(/*TODO*/);
	void requestWindow(/*TODO*/);
	void respondRead(Data *data);
	void respondPolytope(Data *data);
	void responsePolytope(Data *data);
	void responseCommand(Command *command);
	void responseWrite(Data *data);
	void respondSystem(Data *data);
	void responseSystem(Data *data);
	void responseWindow(Command *command);
	void processRead(Data *data, void (Script::*respond)(Data *data));
	void processPolytope(Data *data, void (Script::*respond)(Data *data));
	void processedPolytope(Data *data, void (Script::*respond)(Data *data));
	void processedCommand(Command *command, void (Script::*respond)(Command *command));
	void processedWrite(Data *data, void (Script::*respond)(Data *data));
	void processSystem(Data *data, void (Script::*respond)(Data *data));
	void processedSystem(Data *data, void (Script::*respond)(Data *data));
	void processedWindow(Command *command, void (Script::*respond)(Command *command));
	void callbackCommand(Command *command, void (Script::*respond)(Command *command));
	void callbackData(Data *data, void (Script::*respond)(Data *data));
	void processCommands(Message<Command*> &message, void (Script::*process)(Command *command,
		void (Script::*respond)(Command *command)), void (Script::*respond)(Command *command));
	void processDatas(Message<Data*> &message, void (Script::*process)(Data *command,
		void (Script::*respond)(Data *command)), void (Script::*respond)(Data *data));
public:
	Message<Data*> read2req;
	Message<Data*> polytope2rsp;
	Message<Data*> polytope2req;
	Message<Command*> command2rsp;
	Message<Data*> write2rsp;
	Message<Data*> system2rsp;
	Message<Data*> system2req;
	Message<Command*> window2rsp;
	Script(int n);
	virtual ~Script();
	void connect(int i, Read *ptr);
	void connect(Window *ptr);
	void connect(int i, Polytope *ptr);
	void connect(System *ptr);
	void connect(int i, Write *ptr);
private:
	virtual void init();
	virtual void call();
	virtual void done();
};
