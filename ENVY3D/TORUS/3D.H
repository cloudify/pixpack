#ifndef _3DH_
#define _3DH_

#include "fixed.h"
#include "trig.h"

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;

typedef struct {
	long x, y, z;
} P3D;

typedef P3D VERTEX;
typedef P3D VECTOR;

typedef struct {
	FIXED u, v;
} P2D;

typedef struct {
	VERTEX *Vertex[4];
	VECTOR *Normal;
	VECTOR *MVertex[4];
	VERTEX Center;
} FACE;

#define _MAXVERTEX 64
#define _MAXFACE 64
#define _MAXVECTOR 64

#endif