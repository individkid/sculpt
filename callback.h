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

extern int pierceInit;
extern float piercePoint[3];
extern int piercePlane;
extern int pierceFile;
extern float cursorDelta[2];
extern float cursorPoint[2];
extern float rollerDelta;
extern int transformToggle;
extern enum TargetMode targetMode;
extern enum FixedMode fixedMode;
extern int clickToggle;
extern enum ClickMode clickMode;
extern enum MouseMode mouseMode;
extern enum RollerMode rollerMode;
extern int sessionInit;
extern float sessionMatrix[16];
extern int fileCount;
extern int *fileInit;
extern float (*fileMatrix)[16];
extern int planeInit;
extern float planeMatrix[16];
extern int planeSelect;
extern int fileSelect;
extern int lastSessionInit;
extern float lastSessionMatrix[16];
extern int *lastFileInit;
extern float (*lastFileMatrix)[16];
extern int lastPlaneInit;
extern float lastPlaneMatrix[16];
extern int lastPlaneSelect;
extern int lastFileSelect;
extern struct Command redrawCommand;
extern int testGoon;

void displayError(int error, const char *description);
void displayKey(struct GLFWwindow* ptr, int key, int scancode, int action, int mode);
void displayCursor(struct GLFWwindow *ptr, double xpos, double ypos);
