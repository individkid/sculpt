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
	int tagbits;
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
	int plane;
	int file;
};
struct Mode
{
	int toggle;
	enum ClickMode click;
	enum TargetMode target;
	enum MouseMode mouse;
	enum RollerMode roller;
	enum TopologyMode topology;
	enum FixedMode fixed;
};

void warpCursor(float *cursor);
int decodeClick(int button, int action, int mods);
void sendState(int file, enum Change change, float *matrix);
void sendSculpt(int file, int plane, enum Configure conf);
void sendRefine(int file, int plane, float *pierce);
void sendRelative(int file, int plane, enum TopologyMode topology, float *pierce);
void sendAbsolute(int file, int plane, enum TopologyMode topology);
void sendFacet(int file, int plane, float *matrix);
void sendInvoke(int file, int plane, char key);
void maybeKill(int seq);

int isSuspend();
void globalInit(int nfile);
void getUniform(struct Update *update);
void firstUniform(struct Update *update);
void putUniform(struct Update *update);
void checkQuery(struct Update *update);
void changeState(struct Command *command);
void displayError(int error, const char *description);
void displayKey(struct GLFWwindow* ptr, int key, int scancode, int action, int mode);
void displayCursor(struct GLFWwindow *ptr, double xpos, double ypos);
void displayScroll(struct GLFWwindow *ptr, double xoffset, double yoffset);
void displayClick(struct GLFWwindow *ptr, int button, int action, int mods);
