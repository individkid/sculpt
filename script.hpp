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

#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include "message.hpp"

class Read;
class Window;
class Polytope;
class System;

class Script : public Thread
{
private:
	int nfile;
	// Read->Data->Script
	Message<Data*> **rsp2data2read;
	// Window->Invoke->Script
	Message<Invoke*> *rsp2invoke2window;
	// Script->Question->Polytope
	Message<Question*> **req2question2polytope;
	// System->Invoke->Script
	Message<Invoke*> *rsp2invoke2system;
public:
	// Read->Data->Script
	Message<Data*> read2data2req;
	// Window->Invoke->Script
	Message<Invoke*> window2invoke2req;
	// Script->Question->Polytope
	Message<Question*> polytope2question2rsp;
	// System->Invoke->Script
	Message<Invoke*> system2invoke2req;
	Script(int n);
	void connect(int i, Read *ptr);
	void connect(Window *ptr);
	void connect(int i, Polytope *ptr);
	void connect(System *ptr);
	virtual void call();
};

#endif
