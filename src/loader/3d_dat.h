#pragma once
#ifndef FS_LIB_LOADER_UNKNWON_DAT
#define FS_LIB_LOADER_UNKNWON_DAT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glut.h>

#include "../utils/fs_stdfunc.h"

GLuint loadDAT(const char* path);
GLuint loadSOR();

#endif