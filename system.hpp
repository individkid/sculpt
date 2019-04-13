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

class Read;
class Script;

class System : public Thread
{
private:
	int nfile; int cleanup;
	Message<Data*> **rsp2read;
	Message<Data*> *req2script;
	Message<Data*> *rsp2script;
	void processDatas(Message<Data*> &message);
public:
	Message<Data*> read2req;
	Message<Data*> script2rsp;
	Message<Data*> script2req;
	System(int n);
	virtual ~System();
	void connect(int i, Read *ptr);
	void connect(Script *ptr);
private:
	virtual void init();
	virtual void call();
	virtual void done();
};
