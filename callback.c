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

static struct Current current = {0};
static struct Current warp = {0};
static struct Current *pointer = 0;
static struct Matrix matrix = {0};
static struct Matrix last = {0};
static struct Mode mode = {0};

int isSuspend()
{
	return (mode.click != TransformMode);
}

void globalInit(int nfile)
{
	unitvec(current.normal,3,0);
	unitvec(warp.normal,3,0);
	identmat(matrix.session,4);
    matrix.polytope = malloc(sizeof(float)*nfile*16); for (int i = 0; i < nfile; i++) identmat(matrix.polytope[i],4);
    identmat(matrix.facet,4);
	identmat(last.session,4);
    last.polytope = malloc(sizeof(float)*nfile*16); for (int i = 0; i < nfile; i++) identmat(last.polytope[i],4);
    identmat(last.facet,4);
    pointer = &warp;
    mode.click = PerformMode;
    mode.target = SessionMode;
    mode.mouse = RotateMode;
    mode.roller = CylinderMode;
    mode.topology = NumericMode;
    mode.fixed = RelativeMode;
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
	before[3] = -pointer->pierce[0]; before[7] = -pointer->pierce[1]; before[11] = -pointer->pierce[2];
	after[3] = pointer->pierce[0]; after[7] = pointer->pierce[1]; after[11] = pointer->pierce[2];
	return jumpmat(timesmat(matrix,before,4),after,4);
}

float *angleMatrix(float *matrix)
{
	float angle = ANGLE*pointer->roller;
	float sinval = sinf(angle); float cosval = cosf(angle);
	matrix[0] = cosval; matrix[1] = -sinval; matrix[2] = 0.0;
	matrix[3] = sinval; matrix[4] = cosval; matrix[5] = 0.0;
	matrix[6] = 0.0; matrix[7] = 0.0; matrix[8] = 1.0;
	return matrix;
}

float *tangentPoint(float *point)
{
	float *normal = pointer->normal;
	float *pierce = pointer->pierce;
	float minus[2]; scalevec(copyvec(minus,pierce,2),-1.0,2);
	float diff[2]; plusvec(copyvec(diff,point,2),minus,2);
	float abs = absval(normal[2]);
	float dot = dotvec(diff,normal,2);
	if (abs < 1.0 && absval(dot) > INVALID*abs) return zerovec(point,3);
	float proj[3]; copyvec(proj,diff,2); proj[2] = -dot/normal[2];
	float length = sqrtf(dotvec(diff,diff,2));
	float len = sqrtf(dotvec(proj,proj,3));
	if (len < 1.0 && length > INVALID*len) return zerovec(point,3);
	float adjust = length/len;
	return scalevec(copyvec(point,proj,3),adjust,3);
	// length = sqrt(diff[0]*diff[0]+diff[1]*diff[1])
	// proj[0]*normal[0] + proj[1]*normal[1] + proj[2]*normal[2] = 0
	// proj[0] = diff[0] proj[1] = diff[1]
	// adjust*sqrt(proj[0]*proj[0]+proj[1]*proj[1]+proj[2]*proj[2]) = length
	// point = adjust*proj
}

float *mouseMatrix(float *matrix)
{
	switch (mode.mouse) {
	case (RotateMode): {
	float inverse[9]; rotateMatrix(inverse,pointer->point,pointer->pierce);
	float rotate[9]; timesmat(rotateMatrix(rotate,pointer->pierce,pointer->cursor),inverse,3);
	identmat(matrix,4); copyary(matrix,rotate,3,4,9);
	return fixedMatrix(matrix);}
	case (TangentMode): {
	float point[3]; tangentPoint(copyvec(point,pointer->point,2));
	float cursor[3]; tangentPoint(copyvec(cursor,pointer->cursor,2));
	float translate[16]; identmat(translate,4); translate[3] = cursor[0]-point[0];
	translate[7] = cursor[1]-point[1]; translate[11] = cursor[2]-point[2];
	return copymat(matrix,translate,4);}
	case (TranslateMode): {
	float translate[16]; identmat(translate,4);
	translate[3] = pointer->cursor[0]-pointer->point[0];
	translate[7] = pointer->cursor[1]-pointer->point[1];
	translate[11] = 0.0;
	return copymat(matrix,translate,4);}
	default: displayError(mode.mouse,"invalid mode.mouse");}
	return matrix;
}

float *rollerMatrix(float *matrix)
{
	switch (mode.roller) {
	case (CylinderMode): {
	float inverse[9]; rotateMatrix(inverse,pointer->point,pointer->pierce);
	float rotate[9]; rotateMatrix(rotate,pointer->pierce,pointer->point);
	float zmat[9]; angleMatrix(zmat);
	timesmat(timesmat(rotate,zmat,3),inverse,3);
	identmat(matrix,4); copyary(matrix,rotate,3,4,9);
	return fixedMatrix(matrix);}
	case (ClockMode): {
	float zmat[9]; angleMatrix(zmat);
	identmat(matrix,4); copyary(matrix,zmat,3,4,9);
	return fixedMatrix(matrix);}
	case (NormalMode): {
	float zero[2]; zerovec(zero,2);
	int sub = 0; float min = INVALID;
	for (int i = 0; i < 3; i++) if (pointer->normal[i] < min) {sub = i; min = pointer->normal[i];}
	float norm[2]; int j = 0; for (int i = 0; i < 3; i++) if (i != sub) norm[j++] = pointer->normal[i];
	float inverse[9]; rotateMatrix(inverse,norm,zero);
	float rotate[9]; rotateMatrix(rotate,zero,norm);
	float zmat[9]; angleMatrix(zmat);
	timesmat(timesmat(rotate,zmat,3),inverse,3);
	identmat(matrix,4); copyary(matrix,rotate,3,4,9);
	return fixedMatrix(matrix);}
	case (ParallelMode): {
	float length = LENGTH*pointer->roller/sqrtf(dotvec(pointer->normal,pointer->normal,3));
	float offset[3]; scalevec(copyvec(offset,pointer->normal,3),length,3);
	float translate[16]; identmat(translate,4);
	translate[3] = offset[0]; translate[7] = offset[1]; translate[11] = offset[2];
	return copymat(matrix,translate,4);}
	case (ScaleMode): {
	scalevec(identmat(matrix,4),powf(BASE,pointer->roller),16);
	return fixedMatrix(matrix);}
	default: displayError(mode.roller,"invalid mode.roller");}
	return matrix;
}

void affineMatrix(int file, float *affine)
{
	switch (mode.target) {case(SessionMode): if (mode.toggle == 0)
	timesmat(timesmat(mouseMatrix(affine),matrix.session,4),matrix.polytope[file],4); else
	timesmat(timesmat(rollerMatrix(affine),matrix.session,4),matrix.polytope[file],4); break;
	case (PolytopeMode): if (file == matrix.file) {if (mode.toggle == 0)
	timesmat(timesmat(mouseMatrix(affine),matrix.polytope[file],4),matrix.session,4); else
	timesmat(timesmat(rollerMatrix(affine),matrix.polytope[file],4),matrix.session,4);} else
	timesmat(timesmat(identmat(affine,4),matrix.session,4),matrix.polytope[file],4); break;
	case (FacetMode): timesmat(timesmat(identmat(affine,4),matrix.session,4),matrix.polytope[file],4); break;
	default: displayError(mode.target,"invalid mode.target");}
}

void sendMatrix()
{
	switch (mode.target) {
	case (SessionMode):
	for (int i = 0; i < 16; i++) last.session[i] = matrix.session[i];
	sendState(matrix.file,GlobalChange,matrix.session); break;
	case (PolytopeMode):
	for (int i = 0; i < 16; i++) last.polytope[matrix.file][i] = matrix.polytope[matrix.file][i];
	sendState(matrix.file,MatrixChange,matrix.polytope[matrix.file]); break;
	case (FacetMode):
	for (int i = 0; i < 16; i++) last.facet[i] = matrix.facet[i];
	sendFacet(matrix.file,matrix.plane,matrix.facet); break;
	default: displayError(mode.target,"invalid mode.target");}
}

void getUniform(struct Update *update)
{
	float *cursor = update->format->cursor;
	float *affine = update->format->affine;
	float *perplane = update->format->perplane;
	// float *basis = update->format->basis;
	// float *cutoff = update->format->cutoff;
	// float *slope = update->format->slope;
	// float *aspect = update->format->aspect;
	// Myuint *enable = update->format->enable;
	MYuint *tagplane = &update->format->tagplane;
	// MYuint *taggraph = &update->format->taggraph;
	copyvec(cursor,current.cursor,2);
	affineMatrix(update->file,affine);
	if (mode.target == FacetMode && update->file == matrix.file) {
	if (mode.toggle == 0) mouseMatrix(perplane); else rollerMatrix(perplane);
	timesmat(perplane,matrix.facet,4); *tagplane = matrix.plane;} else identmat(perplane,4);
}

void firstUniform(struct Update *update)
{
	current.pierce[2] = INVALID;
}

void putUniform(struct Update *update)
{
	for (int i = 0; i < update->size; i++)
	if (current.pierce[2] > update->feedback[i].pierce[2]) {
	for (int j = 0; j < 3; j++) current.pierce[j] = update->feedback[i].pierce[j];
	for (int j = 0; j < 3; j++) current.normal[j] = update->feedback[i].normal[j];
	current.tagbits = update->feedback[i].tagbits;
	current.plane = update->feedback[i].plane;
	current.file = update->file;}
}

void checkQuery(struct Update *update)
{
	if (*update->query >= update->size) update->finish = 0;
}

void adjustSession()
{
	float affine[16]; invmat(copymat(affine,matrix.session,4),4);
	timesmat(timesmat(affine,matrix.polytope[matrix.file],4),matrix.session,4);
	copymat(matrix.polytope[matrix.file],affine,4);
}

void adjustPolytope()
{
	float affine[16]; invmat(copymat(affine,matrix.session,4),4);
	jumpmat(jumpmat(affine,matrix.polytope[matrix.file],4),matrix.session,4);
	copymat(matrix.polytope[matrix.file],affine,4);
}

void foldMatrix()
{
	float affine[16];
	if (mode.toggle == 0) mouseMatrix(affine); else rollerMatrix(affine);
	switch (mode.target) {
	case (SessionMode): jumpmat(matrix.session,affine,4); break;
	case (PolytopeMode): jumpmat(matrix.polytope[matrix.file],affine,4); break;
	case (FacetMode): jumpmat(matrix.facet,affine,4); break;
	default: displayError(mode.target,"invalid mode.target");}
	copyvec(pointer->point,pointer->cursor,2);
	pointer->roller = 0;
}

void changeClick(enum ClickMode mod)
{
	foldMatrix(); if (mode.click == TransformMode) sendMatrix();
	if (mode.click == TransformMode && mod != TransformMode) {
	warp = current; pointer = &warp;}
	if (mode.click == SuspendMode && mod == TransformMode) {
	current = warp; pointer = &current; warpCursor(current.cursor);}
	if (mode.click == PierceMode && mod == TransformMode && mode.target != PolytopeMode) {
	pointer = &current; matrix.plane = current.plane; matrix.file = current.file;}
	if (mode.click == PierceMode && mod == TransformMode && mode.target == PolytopeMode) {
	pointer = &current; adjustSession(); matrix.plane = current.plane; matrix.file = current.file; adjustPolytope();}
	mode.click = mod;
}

void changeRoller(enum RollerMode mod)
{
	foldMatrix(); if (mode.click == TransformMode) sendMatrix();
	mode.roller = mod;
}

void changeMouse(enum MouseMode mod)
{
	foldMatrix(); if (mode.click == TransformMode) sendMatrix();
	mode.mouse = mod;
}

void changeTarget(enum TargetMode mod)
{
	foldMatrix(); if (mode.click == TransformMode) sendMatrix();
	if (mode.target == PolytopeMode && mod != PolytopeMode) adjustSession();
	if (mode.target != PolytopeMode && mod == PolytopeMode) adjustPolytope();
	mode.target = mod;
}

void changeTopology(enum TopologyMode mod)
{
	mode.topology = mod;
}

void changeFixed(enum FixedMode mod)
{
	mode.fixed = mod;
}

void changeState(struct Command *command)
{
	switch (command->source) {
	case (ModeSource): switch (command->sculpt) {
	case (ClickUlpt): changeClick(command->click); break;
	case (MouseUlpt): changeMouse(command->mouse); break;
	case (RollerUlpt): changeRoller(command->roller); break;
	case (TargetUlpt): changeTarget(command->target); break;
	case (TopologyUlpt): changeTopology(command->topology); break;
	case (FixedUlpt): changeFixed(command->fixed); break;
	default: displayError(command->sculpt,"invalid command->sculpt");}
	case (MatrixSource): {
	float invert[16]; invmat(copymat(invert,last.polytope[command->file],4),4);
	float delta[16]; timesmat(copymat(delta,matrix.polytope[command->file],4),invert,4);
	timesmat(copymat(matrix.polytope[command->file],command->matrix,4),delta,4); break;}
	case (GlobalSource): {
	float invert[16]; invmat(copymat(invert,last.session,4),4);
	float delta[16]; timesmat(copymat(delta,matrix.session,4),invert,4);
	timesmat(copymat(matrix.session,command->matrix,4),delta,4); break;}
	default: displayError(command->source,"invalid command->source");}
}

void changeToggle(int toggle)
{
	foldMatrix(); if (mode.click == TransformMode) sendMatrix();
	mode.toggle = toggle;
}

void triggerAction()
{
	switch (mode.click) {
	case (AdditiveMode): sendSculpt(current.file,current.plane,AdditiveConf); break;
	case (SubtractiveMode): sendSculpt(current.file,current.plane,SubtractiveConf); break;
	case (RefineMode): sendRefine(current.file,current.plane,current.pierce); break;	
	case (TweakMode): switch (mode.fixed) {
	case (RelativeMode): sendRelative(current.file,current.plane,mode.topology,current.pierce); break;	
	case (AbsoluteMode): sendAbsolute(current.file,current.plane,mode.topology); break;
	default: displayError(mode.fixed,"invalid mode.fixed");}
	case (PerformMode): sendSculpt(current.file,current.plane,PerformConf); break;
	case (TransformMode): changeClick(PierceMode); break;
	case (SuspendMode): case (PierceMode): changeClick(TransformMode); break;
	default: displayError(mode.click,"invalid mode.click");}
}

void toggleSuspend()
{
	if (mode.click == TransformMode) changeClick(SuspendMode);
	else if (mode.click == SuspendMode) changeClick(TransformMode);
	else changeClick(mode.click);
}

void displayError(int error, const char *description)
{
    fprintf(stderr,"GLFW error %d %s\n",error,description);
    exit(-1);
}

void displayKey(struct GLFWwindow* ptr, int key, int scancode, int action, int mods)
{
    if ((DEBUG & CALLBACK_DEBUG) && action == 1) printf("GLFW key %d %d %d %d\n",key,scancode,action,mods);
    if (key == 256 && action == 1) maybeKill(1);
    else if (key == 257 && action == 1) maybeKill(2);
    else if (action == 1) maybeKill(0);
}

void displayCursor(struct GLFWwindow *ptr, double xpos, double ypos)
{
	changeToggle(0);
	current.cursor[0] = xpos; current.cursor[1] = ypos;
}

void displayScroll(struct GLFWwindow *ptr, double xoffset, double yoffset)
{
	changeToggle(1);
	current.roller += yoffset;
}

void displayClick(struct GLFWwindow *ptr, int button, int action, int mods)
{
	int click = decodeClick(button,action,mods);
	if (click == 0) triggerAction();
	if (click == 1) toggleSuspend();
}
