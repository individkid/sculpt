/*
*    callback.c handlers for user manipulation
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

#include <stdio.h>

#include "types.h"

int pierceInit = 0;
float piercePoint[3];
int piercePlane;
int pierceFile;
float pierceCursor[2];
float cursorPoint[2];
float rollerDelta = 0;
int transformToggle = 0;
enum TargetMode targetMode = SessionMode;
enum FixedMode fixedMode = NumericMode;
int clickToggle = 0;
enum ClickMode clickMode = TransformMode;
enum MouseMode mouseMode = RotateMode;
enum RollerMode rollerMode = CylinderMode;
int sessionInit = 0;
float sessionMatrix[16];
int fileCount;
int *fileInit;
float (*fileMatrix)[16];
int planeInit = 0;
float planeMatrix[16];
int planeSelect;
int fileSelect;
int lastSessionInit = 0;
float lastSessionMatrix[16];
int *lastFileInit;
float (*lastFileMatrix)[16];
int lastPlaneInit = 0;
float lastPlaneMatrix[16];
int lastPlaneSelect;
int lastFileSelect;
struct Command redrawCommand = {0};
int testGoon = 1;

void displayError(int error, const char *description)
{
    fprintf(stderr,"GLFW error %d %s\n",error,description);
}

void displayKey(struct GLFWwindow* ptr, int key, int scancode, int action, int mods)
{
    if (action == 1) printf("GLFW key %d %d %d %d\n",key,scancode,action,mods);
    if (key == 256 && action == 1 && testGoon == 1) testGoon = 2;
    if (key == 257 && action == 1 && testGoon == 2) testGoon = 0;
}
