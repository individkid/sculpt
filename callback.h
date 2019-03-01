/*
*    callback.h handlers for user manipulation
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

#include "types.h"

struct Current
{
	float pierce[3];
	float normal[3];
	int plane;
	int file;
	float point[2]; // amount of cursor rolled into matrix
	float cursor[2]; // changed by displayCursor
	float roller; // changed by displayScroll
};
struct Matrix
{
	float session[16];
	float (*polytope)[16];
	float facet[16];
	float align[9];
	int plane;
	int file;
};
struct State
{
	int suspend;
	int toggle;
	enum ClickMode click;
	enum TargetMode target;
	enum MouseMode mouse;
	enum RollerMode roller;
	enum FixedMode fixed;
};

extern int testGoon;

void globalInit(int nfile);
void displayError(int error, const char *description);
void displayKey(struct GLFWwindow* ptr, int key, int scancode, int action, int mode);
void displayCursor(struct GLFWwindow *ptr, double xpos, double ypos);
void displayScroll(struct GLFWwindow *ptr, double xoffset, double yoffset);
