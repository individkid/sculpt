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

Polytope::Polytope(int i) : Thread(),
	read2data2req(this), window2data2req(this),
	write2data2rsp(this), window2command2rsp(this),
	script2question2req(this)
{
}

void Polytope::connect(Read *ptr)
{
	rsp2data2read = &ptr->polytope2data2rsp;
}

void Polytope::connect(Window *ptr)
{
	rsp2data2window = &ptr->polytope2data2rsp;
	req2command2window = &ptr->polytope2command2req;
}

void Polytope::connect(Write *ptr)
{
	req2data2write = &ptr->polytope2data2req;
}

void Polytope::connect(Script *ptr)
{
	rsp2question2script = &ptr->polytope2question2rsp;
}

void Polytope::call()
{
    Data *data = 0; while (read2data2req.get(data)) {
    printf("%s",data->text); rsp2data2read->put(data);}
}
