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

struct Current current = {0};
struct Current warp = {0};
struct Matrix matrix = {0};
struct Matrix last = {0};
struct State state = {0};
int fileCount = 0;
int testGoon = 1;

void globalInit(int nfile)
{
	fileCount = nfile;
	identmat(matrix.session,4);
    matrix.polytope = malloc(sizeof(float)*nfile*16); for (int i = 0; i < nfile; i++) identmat(matrix.polytope[i],4);
    identmat(matrix.facet,4);
    identmat(matrix.align,3);
	identmat(last.session,4);
    last.polytope = malloc(sizeof(float)*nfile*16); for (int i = 0; i < nfile; i++) identmat(last.polytope[i],4);
    identmat(last.facet,4);
    identmat(last.align,3);
    state.click = TransformMode;
    state.target = SessionMode;
    state.mouse = RotateMode;
    state.roller = CylinderMode;
    state.fixed = NumericMode;
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
	before[3] = -current.pierce[0]; before[7] = -current.pierce[1]; before[11] = -current.pierce[2];
	after[3] = current.pierce[0]; after[7] = current.pierce[1]; after[11] = current.pierce[2];
	return jumpmat(timesmat(matrix,before,4),after,4);
}

float *mouseMatrix(float *matrix)
{
	struct Current *pointer;
	if (state.suspend) pointer = &warp; else pointer = &current;
	switch (state.mouse) {
	case (RotateMode): {
	float inverse[9]; rotateMatrix(inverse,pointer->point,pointer->pierce);
	float rotate[9]; timesmat(rotateMatrix(rotate,pointer->pierce,pointer->cursor),inverse,3);
	identmat(matrix,4); copyary(matrix,rotate,3,4,9);
	return fixedMatrix(matrix);}
	case (TranslateMode): {
	float translate[16]; identmat(translate,4);
	translate[3] = pointer->cursor[0]-pointer->point[0];
	translate[7] = pointer->cursor[1]-pointer->point[1];
	translate[11] = 0.0;
	return copymat(matrix,translate,4);}
	default: displayError(state.mouse,"invalid state.mouse"); exit(-1);}
}

float *rollerMatrix(float *matrix)
{
	struct Current *pointer;
	if (state.suspend) pointer = &warp; else pointer = &current;
	switch (state.roller) {
	case (CylinderMode): {
	float inverse[9]; rotateMatrix(inverse,pointer->point,pointer->pierce);
	float rotate[9]; rotateMatrix(rotate,pointer->pierce,pointer->point);
	float zmat[9]; float sinval = sinf(pointer->roller); float cosval = cosf(pointer->roller);
	zmat[0] = cosval; zmat[1] = -sinval; zmat[2] = 0.0;
	zmat[3] = sinval; zmat[4] = cosval; zmat[5] = 0.0;
	zmat[6] = 0.0; zmat[7] = 0.0; zmat[8] = 1.0;
	timesmat(timesmat(rotate,zmat,3),inverse,3);
	identmat(matrix,4); copyary(matrix,rotate,3,4,9);
	return fixedMatrix(matrix);}
	case (ClockMode): {
	float zmat[9]; float sinval = sinf(pointer->roller); float cosval = cosf(pointer->roller);
	zmat[0] = cosval; zmat[1] = -sinval; zmat[2] = 0.0;
	zmat[3] = sinval; zmat[4] = cosval; zmat[5] = 0.0;
	zmat[6] = 0.0; zmat[7] = 0.0; zmat[8] = 1.0;
	identmat(matrix,4); copyary(matrix,zmat,3,4,9);
	return fixedMatrix(matrix);}
	default: displayError(state.roller,"invalid state.roller"); exit(-1);}
	return matrix;
}

void affineMatrix(int file, float *affine)
{
	switch (state.target) {case(SessionMode): if (state.toggle == 0)
	timesmat(timesmat(mouseMatrix(affine),matrix.session,4),matrix.polytope[file],4); else
	timesmat(timesmat(rollerMatrix(affine),matrix.session,4),matrix.polytope[file],4); break;
	case (PolytopeMode): if (file == current.file) {if (state.toggle == 0)
	timesmat(timesmat(mouseMatrix(affine),matrix.polytope[file],4),matrix.session,4); else
	timesmat(timesmat(rollerMatrix(affine),matrix.polytope[file],4),matrix.session,4);} else
	timesmat(timesmat(identmat(affine,4),matrix.session,4),matrix.polytope[file],4); break;
	case (FacetMode): timesmat(timesmat(identmat(affine,4),matrix.session,4),matrix.polytope[file],4); break;
	default: displayError(state.target,"invalid state.target"); exit(-1);}
}

void getUniform(int file, struct Update *update)
{
	float *cursor = update->format->cursor;
	float *affine = update->format->affine;
	float *perplane = update->format->perplane;
	float *perlast = update->format->perlast;
	MYuint *tagplane = &update->format->tagplane;
	MYuint *tagast = &update->format->taglast;
	// MYuint *taggraph = &update->format->taggraph;
	// float *cutoff = update->format->cutoff;
	// float *slope = update->format->slope;
	// float *aspect = update->format->aspect;
	copyvec(cursor,current.cursor,2);
	affineMatrix(file,affine);
	if (state.target == FacetMode && file == current.file) {
	if (state.toggle == 0) mouseMatrix(perplane); else rollerMatrix(perplane);
	timesmat(perplane,matrix.facet,4); *tagplane = current.plane;} else identmat(perplane,4);
	if (file == last.file) {copymat(perlast,last.facet,4); *tagast = last.plane;} else identmat(perlast,4);
}

void foldMatrix()
{
	float affine[16];
	if (state.toggle == 0) mouseMatrix(affine); else rollerMatrix(affine);
	switch (state.target) {
	case (SessionMode): jumpmat(matrix.session,affine,4); break;
	case (PolytopeMode): jumpmat(matrix.polytope[current.file],affine,4); break;
	case (FacetMode): jumpmat(matrix.facet,affine,4); break;
	default: displayError(state.target,"invalid state.target"); exit(-1);}
	copyvec(current.point,current.cursor,2);
	current.roller = 0;
}

void warpCursor(float *cursor);
void changeSuspend()
{
	foldMatrix();
	if (state.suspend == 0) {warp = current; state.suspend = 1;} else {
	current = warp; state.suspend = 0; warpCursor(current.pierce);}
}

void changeToggle()
{
	foldMatrix();
	if (state.toggle == 0) state.toggle = 1; else state.toggle = 0;
}

void changeTarget(enum TargetMode mode)
{
	foldMatrix();
	if (state.target == PolytopeMode && mode != PolytopeMode) {
	float affine[16]; invmat(copymat(affine,matrix.session,4),4);
	timesmat(timesmat(affine,matrix.polytope[current.file],4),matrix.session,4);
	copymat(matrix.polytope[current.file],affine,4);}
	if (state.target != PolytopeMode && mode == PolytopeMode) {
	float affine[16]; invmat(copymat(affine,matrix.session,4),4);
	jumpmat(jumpmat(affine,matrix.polytope[current.file],4),matrix.session,4);
	copymat(matrix.polytope[current.file],affine,4);}
	state.target = mode;
}

void changeClick(enum ClickMode mode)
{
	foldMatrix();
	state.click = mode;
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
	current.cursor[0] = xpos; current.cursor[1] = ypos;
}

void displayScroll(struct GLFWwindow *ptr, double xoffset, double yoffset)
{
	current.roller += yoffset;
}
