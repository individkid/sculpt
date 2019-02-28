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

float piercePoint[3] = {0};
float pierceNormal[3] = {0};
int piercePlane = 0;
int pierceFile = 0;
float warpPoint[3] = {0};
float warpNormal[3] = {0};
int warpPlane = 0;
int warpFile = 0;
int swapPlane = 0;
int swapFile = 0;
float swapMatrix[16] = {0};
float cursorPoint[2] = {0};
float cursorDelta[2] = {0}; // changed by displayCursor
float rollerDelta = 0; // changed by displayScroll
// states:
// 1) clickMode != TransformMode && clickToggle == 0
// 2) clickMode == TransformMode && clickToggle == 0
// 3) clickMode == TransformMode && clickToggle == 1
int clickToggle = 0;
enum ClickMode clickMode = TransformMode;
enum TargetMode targetMode = SessionMode; // state 2 substate
enum FixedMode fixedMode = NumericMode; // state 1 substate
enum MouseMode mouseMode = RotateMode; // state 2 substate
enum RollerMode rollerMode = CylinderMode; // state 2 substate
int transformToggle = 0; // state 2 substate
float sessionMatrix[16] = {0};
int fileCount = 0;
float (*fileMatrix)[16] = {0};
float planeMatrix[16] = {0};
float alignMatrix[9] = {0};
float lastSessionMatrix[16] = {0};
float (*lastFileMatrix)[16] = {0};
float lastPlaneMatrix[16] = {0};
float lastAlignMatrix[9] = {0};
int lastPiercePlane = 0;
int lastPierceFile = 0;
struct Command redrawCommand = {0};
struct Command pierceCommand = {0};
int testGoon = 1;

void globalInit(int nfile)
{
	fileCount = nfile;
	identmat(sessionMatrix,4);
    fileMatrix = malloc(sizeof(float)*nfile*16); for (int i = 0; i < nfile; i++) identmat(fileMatrix[i],4);
    identmat(planeMatrix,4);
    identmat(alignMatrix,3);
    identmat(lastSessionMatrix,4);
    lastFileMatrix = malloc(sizeof(float)*nfile*16); for (int i = 0; i < nfile; i++) identmat(lastFileMatrix[i],4);
    identmat(lastPlaneMatrix,4);
    identmat(lastAlignMatrix,3);
}

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
	float inverse[9]; rotateMatrix(inverse,cursorPoint,piercePoint);
	float rotate[9]; timesmat(rotateMatrix(rotate,piercePoint,cursorDelta),inverse,3);
	identmat(matrix,4); copyary(matrix,rotate,3,4,9);
	return fixedMatrix(matrix);}
	case (TranslateMode): {
	float translate[16]; identmat(translate,4);
	translate[3] = cursorDelta[0]-cursorPoint[0];
	translate[7] = cursorDelta[1]-cursorPoint[1];
	translate[11] = 0.0;
	return copymat(matrix,translate,4);}
	default: displayError(mouseMode,"invalid mouseMode"); exit(-1);}
}

float *rollerMatrix(float *matrix)
{
	switch (rollerMode) {
	case (CylinderMode): {
	float inverse[9]; rotateMatrix(inverse,cursorPoint,piercePoint);
	float rotate[9]; rotateMatrix(rotate,piercePoint,cursorPoint);
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
	float *cursor = update->format->cursor;
	float *affine = update->format->affine;
	float *perplane = update->format->perplane;
	float *perswap = update->format->perswap;
	MYuint *tagplane = &update->format->tagplane;
	MYuint *tagswap = &update->format->tagswap;
	// MYuint *taggraph = &update->format->taggraph;
	// float *cutoff = update->format->cutoff;
	// float *slope = update->format->slope;
	// float *aspect = update->format->aspect;
	copyvec(cursor,cursorDelta,2);
	if (clickMode != TransformMode || clickToggle == 1 || targetMode == FacetMode)
	identmat(affine,4); else affineMatrix(affine);
	if (targetMode == PolytopeMode && file == pierceFile)
	timesmat(timesmat(affine,fileMatrix[file],4),sessionMatrix,4); else
	timesmat(timesmat(affine,sessionMatrix,4),fileMatrix[file],4);
	if (clickMode != TransformMode || clickToggle == 1 || targetMode != FacetMode || file != pierceFile)
	identmat(perplane,4); else affineMatrix(perplane);
	if (targetMode == FacetMode && file == pierceFile) {timesmat(perplane,planeMatrix,4); *tagplane = piercePlane;}
	if (file == swapFile) {copymat(perswap,swapMatrix,4); *tagswap = swapPlane;} else identmat(perswap,4);
}

enum ClickMode clearClickToggle()
{
	if (clickMode != TransformMode) return clickMode;
	copyvec(cursorPoint,piercePoint,2);
	copyvec(cursorDelta,piercePoint,2);
	rollerDelta = 0;
	clickToggle = 0;
	swapFile = pierceFile;
	swapPlane = piercePlane;
	copymat(swapMatrix,planeMatrix,4);
	return TransformMode;
}

int changeClickToggle(float *xpos, float *ypos)
{
	if (clickMode != TransformMode) return 0;
	if (clickToggle == 0) {
	// transition from state 2 to 3
	copyvec(warpPoint,piercePoint,3);
	copyvec(warpNormal,pierceNormal,3);
	warpPlane = piercePlane;
	warpFile = pierceFile;
	clickToggle = 1;
	return 0;} else {
	// transition from state 3 to 2
	copyvec(piercePoint,warpPoint,3);
	copyvec(pierceNormal,warpNormal,3);
	piercePlane = warpPlane;
	pierceFile = warpFile;
	*xpos = piercePoint[0]; *ypos = piercePoint[1];
	clickToggle = 0;
	return 1;}
}

void changeTransformToggle()
{
	if (clickMode != TransformMode) return;
	float *matrix; switch (targetMode) {
	case (SessionMode): matrix = sessionMatrix; break;
	case (PolytopeMode): matrix = fileMatrix[pierceFile]; break;
	case (FacetMode): matrix = planeMatrix; break;
	default: displayError(targetMode,"invalid targetMode"); exit(-1);}
	float affine[16]; jumpmat(matrix,affineMatrix(affine),4);
	if (transformToggle == 0) {
	copyvec(cursorPoint,cursorDelta,2);
	transformToggle = 1;} else {
	rollerDelta = 0;
	transformToggle = 0;}
}

void changeTargetMode(enum TargetMode mode)
{
	if (targetMode == PolytopeMode && mode != PolytopeMode) {
	float matrix[16]; invmat(copymat(matrix,sessionMatrix,4),4);
	timesmat(timesmat(matrix,fileMatrix[pierceFile],4),sessionMatrix,4);
	copymat(fileMatrix[pierceFile],matrix,4);}
	if (targetMode != PolytopeMode && mode == PolytopeMode) {
	float matrix[16]; invmat(copymat(matrix,sessionMatrix,4),4);
	jumpmat(jumpmat(matrix,fileMatrix[pierceFile],4),sessionMatrix,4);
	copymat(fileMatrix[pierceFile],matrix,4);}
	if (targetMode != mode) clearClickToggle();
	targetMode = mode;
}

void changeClickMode(enum ClickMode mode)
{
	if (clickMode == TransformMode && mode != TransformMode)
	clearClickToggle(); // transition state from 2 or 3 to 1
	clickMode = mode;
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
	cursorDelta[0] = xpos; cursorDelta[1] = ypos;
}

void displayScroll(struct GLFWwindow *ptr, double xoffset, double yoffset)
{
	rollerDelta += yoffset;
}
