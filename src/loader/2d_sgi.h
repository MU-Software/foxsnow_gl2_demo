#pragma once
#ifndef FS_LIB_LOADER_SILICON_GRAPHICS_SGI
#define FS_LIB_LOADER_SILICON_GRAPHICS_SGI

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <GL/glut.h>

typedef struct _ImageRec {
	unsigned short imagic;
	unsigned short type;
	unsigned short dim;
	unsigned short xsize, ysize, zsize;
	unsigned int min, max;
	unsigned int wasteBytes;
	char name[80];
	unsigned long colorMap;
	FILE* file;
	unsigned char* tmp, * tmpR, * tmpG, * tmpB;
	unsigned long rleEnd;
	unsigned int* rowStart;
	int* rowSize;
} ImageRec;

GLuint loadTEX(char* name);
GLuint loadSGI(char* name);

#endif