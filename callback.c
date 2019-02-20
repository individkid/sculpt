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
#include "arithmetic.h"

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

void changeTargetMode(enum TargetMode mode)
{
	if (targetMode == PolytopeMode && mode != PolytopeMode) {
		// change B to X, such that AX = BA
	}
	if (targetMode != PolytopeMode && mode == PolytopeMode) {
		// change B to X, such that XA = AB
	}
	targetMode = mode;
}

void getUniform(int file, struct Update *update)
{
	float *affine = update->format->affine;
	float *perplane = update->format->perplane;
	MYuint *tagplane = &update->format->tagplane;
	// MYuint *taggraph = &update->format->taggraph;
	// float *cutoff = update->format->cutoff;
	// float *slope = update->format->cutoff;
	// float *aspect = update->format->aspect;
	if (clickMode != TransformMode || clickToggle == 1 || pierceInit == 0 || targetMode == FacetMode) identmat(affine,4); else {
	// fill affine with pierch,cursor,roller depending on mouseMode and rollerMode
	}
	if (sessionInit == 0) {sessionInit = 1; identmat(sessionMatrix,4);}
	if (fileInit[file] == 0) {fileInit[file] = 1; identmat(fileMatrix[file],4);}
	if (planeInit == 0) {planeInit = 1; identmat(planeMatrix,4);}
	if (targetMode == PolytopeMode && pierceInit == 1 && file == pierceFile)
	timesmat(timesmat(affine,fileMatrix[file],4),sessionMatrix,4); else
	timesmat(timesmat(affine,sessionMatrix,4),fileMatrix[file],4);
	if (clickMode != TransformMode || clickToggle == 1 || pierceInit == 0 || targetMode != FacetMode || file != fileSelect) identmat(perplane,4); else {
	// fill perplane with pierce,cursor,roller depending on mouseMode and rollerMode
	}
	if (targetMode == FacetMode && file == fileSelect) {timesmat(perplane,planeMatrix,4); *tagplane = planeSelect;}
}

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
