/*
*    arithmetic.c functions for operations on float arrays
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

#include "arithmetic.h"

float dotvec(float *u, float *v, int n)
{
    float w = 0;
    for (int i = 0; i < n; i++) w += u[i]*v[i];
    return w;
}

float *plusvec(float *u, float *v, int n)
{
    for (int i = 0; i < n; i++) u[i] += v[i];
    return u;
}

float *scalevec(float *u, float s, int n)
{
    for (int i = 0; i < n; i++) u[i] *= s;
    return u;
}

float *jumpvec(float *u, float *v, int n)
{
    float w[n];
    for (int i = 0; i < n; i++) w[i] = u[i];
    for (int i = 0; i < n; i++) {
        u[i] = 0.0;
        for (int j = 0; j < n; j++) {
            u[i] += v[j*n+i]*w[j];}}
    return u;
}

float *zerovec(float *u, int n)
{
    for (int i = 0; i < n; i++) u[i] = 0.0;
    return u;
}

float *unitvec(float *u, int n, int m)
{
    for (int i = 0; i < n; i++) u[i] = (i == m ? 1.0 : 0.0);
    return u;
}

float *timesmat(float *u, float *v, int n)
{
    int m = n*n; float w[m];
    for (int i = 0; i < m; i++) w[i] = u[i];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            u[i*n+j] = 0.0;
            for (int k = 0; k < n; k++) {
                u[i*n+j] += w[k*n+j]*v[i*n+k];}}}
    return u;
}

float *jumpmat(float *u, float *v, int n)
{
    int m = n*n; float w[m];
    for (int i = 0; i < m; i++) w[i] = u[i];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            u[i*n+j] = 0.0;
            for (int k = 0; k < n; k++) {
                u[i*n+j] += v[k*n+j]*w[i*n+k];}}}
    return u;
}

float *identmat(float *u, int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            u[i*n+j] = (i == j ? 1.0 : 0.0);}}
    return u;
}

float *copyary(float *u, float *v, int duty, int stride, int size)
{
    // duty is number to copy in chunk; negative for number to skip
    // stride is size of chunks
    // size is total number to copy
    float *w = u;
    int i = 0;
    int j = 0;
    int k = 0;
    if (duty == 0 || stride <= 0 || size < 0) return 0;
    while (i < size) {
        if (k == 0) {j = duty; k = stride;}
        if (j > 0 && duty > 0) *w = v[i++];
        if (j == 0 && duty < 0) *w = v[i++];
        w++; k--;
        if (j > 0) j--;
        if (j < 0) j++;}
    return u;
}

float *copyvec(float *u, float *v, int n)
{
    for (int i = 0; i < n; i++) u[i] = v[i];
    return u;
}

float *copymat(float *u, float *v, int n)
{
    return copyvec(u,v,n*n);
}

float *compmat(float *u, float *v, int n)
{
    for (int i = 0; i < n*n; i++) if (u[i] != v[i]) return 0;
    return u;
}

float *crossmat(float *u)
{
    float x = u[0]; float y = u[1]; float z = u[2];
    u[0] =  0; u[3] = -z; u[6] =  y;
    u[1] =  z; u[4] =  0; u[7] = -x;
    u[2] = -y; u[5] =  x; u[8] =  0;
    return u;
}

float *crossvec(float *u, float *v)
{
    float w[9]; copyvec(w,u,3);
    return jumpvec(copyvec(u,v,3),crossmat(w),3);
}

float *submat(float *u, int i, int n)
{
    int m = n*n; int k = 0;
    for (int j = 0; j < m; j++) if (j/n!=i/n && j%n!=i%n) u[k++] = u[j];
    return u;
}

float *minmat(float *u, int n)
{
    int m = n*n; float v[m];
    for (int i = 0; i < m; i++) {
        float w[m]; submat(copymat(w,u,n),i,n);
        v[i%n*n+i/n] = detmat(w,n-1);}
    return copymat(u,v,n);
}

float *cofmat(float *u, int n)
{
    int m = n*n;
    for (int i = 0; i < m; i++)
    u[i] = ((i/n)%2!=(i%n)%2?-u[i]:u[i]);
    return u;
}

float detmat(float *u, int n)
{
    if (n == 1) return *u;
    int m = n*n; float det = 0.0;
    for (int i = 0; i < n; i++) {
    float v[m];
    float s = detmat(submat(copymat(v,u,n),i,n),n-1);
    det += ((i/n)%2!=(i%n)%2?-s:s);}
    return det;
}

float *xposmat(float *u, int n)
{
    int m = n*n; float v[m];
    for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
    v[i*n+j] = u[j*n+i];
    return copyvec(u,v,m);
}

float *adjmat(float *u, int n)
{
    return xposmat(cofmat(u,n),n);
}

float *invmat(float *u, int n)
{
    int m = n*n; float v[m];
    adjmat(copymat(v,u,n),n);
    float det = detmat(u,n);
    float lim = det*INVALID;
    for (int i = 0; i < m; i++) if (det<1.0 && v[i]>lim) {fprintf(stderr,"cannot invert matrix\n"); exit(-1);}
    for (int i = 0; i < m; i++) u[i] = v[i]/det;
    return u;
}

float *crossvecs(float *u, int n)
{
    int m = n*n; int p = n-1; int q = p*n; float w[m];
    for (int i = q; i < m; i++) w[i] = 0.0;
    xposmat(copyvec(w,u,q),n);
    for (int i = 0; i < n; i++) {
    int j = (i+1)*n-1;
    float v[m];
    float s = detmat(submat(copyvec(v,u,m),j,n),n-1);
    w[i] = ((j/n)%2!=(j%n)%2?-s:s);}
    return copyvec(u,w,n);
}

float *tweakvec(float *u, float a, float b, int n)
{
    for (int i = 0; i < n; i++) u[i] = a+((b-a)*rand()/(float)RAND_MAX);
    return u;
}

float absval(float a)
{
    if (a>0.0) return a;
    return -a;
}

float *basearrow(float *u, float *v, int *i, float *b, int n)
{
    // given feather u, arrow v, base points b, dimension n
    // return distances of plane above base points in b
    // and return index of base points in i
    *i = 0;
    for (int j = 1; j < n; j++)
    if (absval(v[j]) > absval(v[*i])) *i = j;
    int k[n];
    for (int j = 0; j < n; j++) k[j] = (*i+j)%n;
    float x[n];
    for (int j = 0; j < n; j++) x[j] = u[k[j]];
    float y[n];
    for (int j = 0; j < n; j++) y[j] = v[k[j]];
    // (x-x[0])*y[0]+(y-x[1])*y[1]+...=0
    for (int h = 0; h < n; h++) {
        float a[n];
        for (int j = 0; j < n; j++) a[j] = b[*i*n*n+h*n+k[j]];
        float w[n-1];
        copyvec(w,x+1,n-1);
        scalevec(w,-1.0,n-1);
        plusvec(w,a+1,n-1);
        u[h] = x[0]-(dotvec(w,y+1,n-1)/y[0])-a[0];}
    return u;
}

float *arrowbase(float *u, float *v, int i, float *b, int n)
{
    // given distances u above index i in base points b
    // return feather in u and arrow in v
    float point[n][n];
    for (int j = 0; j < n; j++) {
    copyvec(point[j],b+i*n*n+j*n,n); point[j][i] = u[j];}
    float diff[n-1][n];
    for (int j = 0; j < n-1; j++)
    plusvec(scalevec(copyvec(diff[j],point[0],n),-1.0,n),point[j+1],n);
    float matrix[(n-1)*n];
    for (int j = 0; j < n-1; j++)
    for (int k = 0; k < n; k++)
    matrix[j*n+k] = diff[j][k];
    crossvecs(matrix,n);
    copyvec(v,matrix,n);
    return copyvec(u,point[0],n);
}
