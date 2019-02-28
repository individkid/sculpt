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

extern float piercePoint[3];
extern float pierceNormal[3];
extern int piercePlane;
extern int pierceFile;
extern float warpPoint[3];
extern float warpNormal[3];
extern int warpPlane;
extern int warpFile;
extern int swapPlane;
extern int swapFile;
extern float swapMatrix[16];
extern float cursorPoint[2];
extern float cursorDelta[2];
extern float rollerDelta;
extern int clickToggle;
extern enum ClickMode clickMode;
extern enum TargetMode targetMode;
extern enum FixedMode fixedMode;
extern enum MouseMode mouseMode;
extern enum RollerMode rollerMode;
extern int transformToggle;
extern float sessionMatrix[16];
extern int fileCount;
extern float (*fileMatrix)[16];
extern float planeMatrix[16];
extern float lastSessionMatrix[16];
extern float (*lastFileMatrix)[16];
extern float lastPlaneMatrix[16];
extern int lastPiercePlane;
extern int lastPierceFile;
extern struct Command redrawCommand;
extern struct Command pierceCommand;
extern int testGoon;

void globalInit(int nfile);
void displayError(int error, const char *description);
void displayKey(struct GLFWwindow* ptr, int key, int scancode, int action, int mode);
void displayCursor(struct GLFWwindow *ptr, double xpos, double ypos);
void displayScroll(struct GLFWwindow *ptr, double xoffset, double yoffset);
