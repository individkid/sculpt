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
#include "write.hpp"

Polytope::Polytope(int i, Window &gl, Write &w) : Thread(), window(gl), write(w), read(this), response(this), action(this)
{
	gl.connect(i,this);
}

void Polytope::call()
{
}
