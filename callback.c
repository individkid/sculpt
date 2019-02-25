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
#include <stdlib.h>
#include <math.h>

#include "callback.h"
#include "arithmetic.h"

int pierceInit = 0;
float piercePoint[3];
int piercePlane;
int pierceFile;
float cursorDelta[2];
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
int fileSelect;
int planeInit = 0;
float planeMatrix[16];
int planeSelect;
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

float *rotateMatrix(float *matrix, float *from, float *to)
{
	float minus[2]; scalevec(copyvec(minus,from,2),-1.0,2);
	float delta[2]; plusvec(copyvec(delta,to,2),minus,2);
	float angle[2]; angle[0] = -sqrtf(delta[0]*delta[0]+delta[1]*delta[1]); angle[1] = 1.0;
	float xmat[9]; float zmat[9]; float zinv[9];
	scalevec(angle,1.0/sqrtf(dotvec(angle,angle,2)),2);
	xmat[0] = 1.0; xmat[1] = xmat[2] = 0.0;
	xmat[3] = 0.0; xmat[4] = angle[1]; xmat[5] = -angle[0];
	xmat[6] = 0.0; xmat[7] = angle[0]; xmat[8] = angle[1];
	float denom = sqrtf(dotvec(delta,delta,2));
	if (absval(denom*INVALID) >= 1.0) {
	scalevec(delta,1.0/denom,2);
	zmat[0] = delta[1]; zmat[1] = -delta[0]; zmat[2] = 0.0;
	zmat[3] = delta[0]; zmat[4] = delta[1]; zmat[5] = 0.0;
	zmat[6] = 0.0; zmat[7] = 0.0; zmat[8] = 1.0;
	zinv[0] = delta[1]; zinv[1] = delta[0]; zinv[2] = 0.0;
	zinv[3] = -delta[0]; zinv[4] = delta[1]; zinv[5] = 0.0;
	zinv[6] = 0.0; zinv[7] = 0.0; zinv[8] = 1.0;
	return timesmat(timesmat(copymat(matrix,zmat,3),xmat,3),zinv,3);} else {
	return copymat(matrix,xmat,3);}
}

float *fixedMatrix(float *matrix)
{
	float before[16]; float after[16]; identmat(before,4); identmat(after,4);
	before[3] = -piercePoint[0]; before[7] = -piercePoint[1]; before[11] = -piercePoint[2];
	after[3] = piercePoint[0]; after[7] = piercePoint[1]; after[11] = piercePoint[2];
	return jumpmat(timesmat(matrix,before,4),after,4);
}

float *mouseMatrix(float *matrix)
{
	switch (mouseMode) {
	case (RotateMode): {
	float inverse[9]; rotateMatrix(inverse,cursorDelta,piercePoint);
	float rotate[9]; timesmat(rotateMatrix(rotate,piercePoint,cursorPoint),inverse,3);
	identmat(matrix,4); copyary(matrix,rotate,3,4,9);
	return fixedMatrix(matrix);}
	case (TranslateMode): {
	float translate[16]; identmat(translate,4);
	translate[3] = cursorPoint[0]-cursorDelta[0];
	translate[7] = cursorPoint[1]-cursorDelta[1];
	translate[11] = 0.0;
	return copymat(matrix,translate,4);}
	default: displayError(mouseMode,"invalid mouseMode"); exit(-1);}
}

float *rollerMatrix(float *matrix)
{
	switch (rollerMode) {
	case (CylinderMode): {
	float inverse[9]; rotateMatrix(inverse,cursorDelta,piercePoint);
	float rotate[9]; rotateMatrix(rotate,piercePoint,cursorDelta);
	float zmat[9]; float sinval = sinf(rollerDelta); float cosval = cosf(rollerDelta);
	zmat[0] = cosval; zmat[1] = -sinval; zmat[2] = 0.0;
	zmat[3] = sinval; zmat[4] = cosval; zmat[5] = 0.0;
	zmat[6] = 0.0; zmat[7] = 0.0; zmat[8] = 1.0;
	timesmat(timesmat(rotate,zmat,3),inverse,3);
	identmat(matrix,4); copyary(matrix,rotate,3,4,9);
	return fixedMatrix(matrix);}
	case (ClockMode): {
	float zmat[9]; float sinval = sinf(rollerDelta); float cosval = cosf(rollerDelta);
	zmat[0] = cosval; zmat[1] = -sinval; zmat[2] = 0.0;
	zmat[3] = sinval; zmat[4] = cosval; zmat[5] = 0.0;
	zmat[6] = 0.0; zmat[7] = 0.0; zmat[8] = 1.0;
	identmat(matrix,4); copyary(matrix,zmat,3,4,9);
	return fixedMatrix(matrix);}
	default: displayError(rollerMode,"invalid rollerMode"); exit(-1);}
	return matrix;
}

float *affineMatrix(float *matrix)
{
	if (transformToggle == 0) return mouseMatrix(matrix);
	else return rollerMatrix(matrix);
}

void getUniform(int file, struct Update *update)
{
	float *affine = update->format->affine;
	float *perplane = update->format->perplane;
	MYuint *tagplane = &update->format->tagplane;
	// MYuint *taggraph = &update->format->taggraph;
	// float *cutoff = update->format->cutoff;
	// float *slope = update->format->slope;
	// float *aspect = update->format->aspect;
	if (clickMode != TransformMode || clickToggle == 1 || pierceInit == 0 || targetMode == FacetMode)
	identmat(affine,4); else affineMatrix(affine);
	if (sessionInit == 0) {sessionInit = 1; identmat(sessionMatrix,4);}
	if (fileInit[file] == 0) {fileInit[file] = 1; identmat(fileMatrix[file],4);}
	if (planeInit == 0) {planeInit = 1; identmat(planeMatrix,4);}
	if (targetMode == PolytopeMode && pierceInit == 1 && file == pierceFile)
	timesmat(timesmat(affine,fileMatrix[file],4),sessionMatrix,4); else
	timesmat(timesmat(affine,sessionMatrix,4),fileMatrix[file],4);
	if (clickMode != TransformMode || clickToggle == 1 || pierceInit == 0 || targetMode != FacetMode || file != fileSelect)
	identmat(perplane,4); else affineMatrix(perplane);
	if (targetMode == FacetMode && file == fileSelect) {timesmat(perplane,planeMatrix,4); *tagplane = planeSelect;}
}

void changeTransformToggle()
{
	if (clickMode != TransformMode) return;
	float *matrix; switch (targetMode) {
	case (SessionMode): matrix = sessionMatrix; break;
	case (PolytopeMode): matrix = fileMatrix[fileSelect]; break;
	case (FacetMode): matrix = planeMatrix; break;
	default: displayError(targetMode,"invalid targetMode"); exit(-1);}
	float affine[16]; jumpmat(matrix,affineMatrix(affine),4);
	if (transformToggle == 0) {
	copyvec(cursorDelta,cursorPoint,2);
	transformToggle = 1;} else {
	rollerDelta = 0;
	transformToggle = 0;}
}

void changeTargetMode(enum TargetMode mode)
{
	if (targetMode == PolytopeMode && mode != PolytopeMode) {
	float matrix[16]; invmat(copymat(matrix,sessionMatrix,4),4);
	timesmat(timesmat(matrix,fileMatrix[fileSelect],4),sessionMatrix,4);
	copymat(fileMatrix[fileSelect],matrix,4);}
	if (targetMode != PolytopeMode && mode == PolytopeMode) {
	float matrix[16]; invmat(copymat(matrix,sessionMatrix,4),4);
	jumpmat(jumpmat(matrix,fileMatrix[fileSelect],4),sessionMatrix,4);
	copymat(fileMatrix[fileSelect],matrix,4);}
	targetMode = mode;
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

void displayCursor(struct GLFWwindow *ptr, double xpos, double ypos)
{
	cursorPoint[0] = xpos; cursorPoint[1] = ypos;
}

void displayScroll(struct GLFWwindow *ptr, double xoffset, double yoffset)
{
	rollerDelta += yoffset;
}
