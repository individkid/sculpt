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

#include <lua.hpp>
#include "script.hpp"
#include "window.hpp"
#include "system.hpp"
#include "read.hpp"
#include "polytope.hpp"
#include "write.hpp"

static Pool<Data> datas(__FILE__,__LINE__);
static Pool<Command> commands(__FILE__,__LINE__);

void Script::requestPolytope(/*TODO*/)
{
	Data *data;
	// TODO get and initialze
	req2polytope[data->file]->put(data);
}

void Script::requestCommand(/*TODO*/)
{
	Command *command;
	// TODO get and initialize
	req2command[command->file]->put(command);
}

void Script::requestWrite(/*TODO*/)
{
	Data *data;
	// TODO get and initialze
	req2write[data->file]->put(data);
}

void Script::requestSystem(/*TODO*/)
{
	Data *data;
	// TODO get and initialze
	req2system->put(data);
}

void Script::requestWindow(/*TODO*/)
{
	Command *command;
	// TODO get and initialize
	req2window->put(command);
}


void Script::respondRead(Data *data)
{
	rsp2read[data->file]->put(data);
}

void Script::respondPolytope(Data *data)
{
	rsp2polytope[data->file]->put(data);
}

void Script::responsePolytope(Data *data)
{
	datas.put(data);
}

void Script::responseCommand(Command *command)
{
	commands.put(command);
}

void Script::responseWrite(Data *data)
{
	datas.put(data);
}

void Script::respondSystem(Data *data)
{
	rsp2system->put(data);
}

void Script::responseSystem(Data *data)
{
	datas.put(data);
}

void Script::responseWindow(Command *command)
{
	commands.put(command);
}

void Script::processRead(Data *data, void (Script::*respond)(Data *data))
{
	// TODO execute script
	(this->*respond)(data);
}

void Script::processPolytope(Data *data, void (Script::*respond)(Data *data))
{
	// TODO write pierce plane file, execute script, and read result
	(this->*respond)(data);
}

void Script::processedPolytope(Data *data, void (Script::*respond)(Data *data))
{
	// TODO write topology, and execute script
	(this->*respond)(data);
}

void Script::processedCommand(Command *command, void (Script::*respond)(Command *command))
{
	(this->*respond)(command);
}

void Script::processedWrite(Data *data, void (Script::*respond)(Data *data))
{
	(this->*respond)(data);
}

void Script::processSystem(Data *data, void (Script::*respond)(Data *data))
{
	// TODO write stocks, execute script, and read metric
	(this->*respond)(data);
}

void Script::processedSystem(Data *data, void (Script::*respond)(Data *data))
{
	// TODO write stocks, and execute script
	(this->*respond)(data);
}

void Script::processedWindow(Command *command, void (Script::*respond)(Command *command))
{
	(this->*respond)(command);
}

void Script::callbackCommand(Command *command, void (Script::*respond)(Command *command))
{
	(this->*respond)(command);
}

void Script::callbackData(Data *data, void (Script::*respond)(Data *data))
{
	(this->*respond)(data);
}

void Script::processCommands(Message<Command*> &message, void (Script::*process)(Command *command,
	void (Script::*respond)(Command *command)), void (Script::*respond)(Command *command))
{
	Command *command; while (message.get(command)) (this->*process)(command,respond);
}

void Script::processDatas(Message<Data*> &message, void (Script::*process)(Data *command,
	void (Script::*respond)(Data *command)), void (Script::*respond)(Data *data))
{
	Data *data; while (message.get(data)) (this->*process)(data,respond);
}

Script::Script(int n) : nfile(n), rsp2read(new Message<Data*>*[n]),
	rsp2polytope(new Message<Data*>*[n]), req2polytope(new Message<Data*>*[n]),
	req2command(new Message<Command*>*[n]), req2write(new Message<Data*>*[n]),
	read2req(this,"Read->Data->Script"), polytope2rsp(this,"Script<-Data<-Polytope"),
	polytope2req(this,"Polytope->Data->Script"), command2rsp(this,"Script<-Command<-Write"),
	write2rsp(this,"Script<-Data<-Write"), system2rsp(this,"Script<-Data<-System"),
	system2req(this,"System->Data->Script"), window2rsp(this,"Script<-Command<-Window")
{
}

Script::~Script()
{
	processDatas(polytope2rsp,&Script::callbackData,&Script::responsePolytope);
	processCommands(command2rsp,&Script::callbackCommand,&Script::responseCommand);
	processDatas(write2rsp,&Script::callbackData,&Script::responseWrite);
	processDatas(system2rsp,&Script::callbackData,&Script::responseSystem);
	processCommands(window2rsp,&Script::callbackCommand,&Script::responseWindow);
}

void Script::connect(int i, Read *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    rsp2read[i] = &ptr->script2rsp;
}

void Script::connect(Window *ptr)
{
	req2window = &ptr->script2req;
}

void Script::connect(int i, Polytope *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
	rsp2polytope[i] = &ptr->script2rsp;
    req2polytope[i] = &ptr->script2req;
}

void Script::connect(System *ptr)
{
	rsp2system = &ptr->script2rsp;
	req2system = &ptr->script2req;
}

void Script::connect(int i, Write *ptr)
{
    if (i < 0 || i >= nfile) error("connect",i,__FILE__,__LINE__);
    req2command[i] = &ptr->command2req;
    req2write[i] = &ptr->script2req;
}

void Script::init()
{
	for (int i = 0; i < nfile; i++) if (rsp2read[i] == 0) error("unconnected rsp2read",i,__FILE__,__LINE__);
	for (int i = 0; i < nfile; i++) if (rsp2polytope[i] == 0) error("unconnected rsp2polytope",i,__FILE__,__LINE__);
	for (int i = 0; i < nfile; i++) if (req2polytope[i] == 0) error("unconnected req2polytope",i,__FILE__,__LINE__);
	for (int i = 0; i < nfile; i++) if (req2command[i] == 0) error("unconnected req2command",i,__FILE__,__LINE__);
	for (int i = 0; i < nfile; i++) if (req2write[i] == 0) error("unconnected req2write",i,__FILE__,__LINE__);
	if (rsp2system == 0) error("unconnected rsp2system",0,__FILE__,__LINE__);
	if (req2system == 0) error("unconnected req2system",0,__FILE__,__LINE__);
	if (req2window == 0) error("unconnected req2window",0,__FILE__,__LINE__);
}

void Script::call()
{
	processDatas(read2req,&Script::processRead,&Script::respondRead);
	processDatas(polytope2rsp,&Script::processedPolytope,&Script::responsePolytope);
	processDatas(polytope2req,&Script::processPolytope,&Script::respondPolytope);
	processCommands(command2rsp,&Script::processedCommand,&Script::responseCommand);
	processDatas(write2rsp,&Script::processedWrite,&Script::responseWrite);
	processDatas(system2rsp,&Script::processedSystem,&Script::responseSystem);
	processDatas(system2req,&Script::processSystem,&Script::respondSystem);
	processCommands(window2rsp,&Script::processedWindow,&Script::responseWindow);
}

void Script::done()
{
	processDatas(read2req,&Script::callbackData,&Script::respondRead);
	processDatas(polytope2req,&Script::callbackData,&Script::respondPolytope);
	processDatas(system2req,&Script::callbackData,&Script::respondSystem);
}
