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

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "message.hpp"

class Read;
class Script;

class System : public Thread
{
private:
	int nfile;
	// Read->Data->System  
	Message<Data*> **rsp2data2read;
	// System->Invoke->Script  
	Message<Invoke*> *req2invoke2script;
public:
	Message<Data*> read2data2req;
	Message<Invoke*> script2invoke2rsp;
	System(int n);
	void connect(int i, Read *ptr);
	void connect(Script *ptr);
	virtual void call();
};

#endif
