#ifndef FS_LIB_STDFUNC
#define FS_LIB_STDFUNC

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#define _USE_MATH_DEFINES
#include <math.h>

#define ALLOC_FAILCHECK(mem) if(!mem){printf("CALLOC FAILED!\n"); exit(1);}
#define NULL_CHECK(mem) if(!mem) {printf("NULL POINTER EXCEPTION!\n"); exit(1);}

typedef struct {
    float x;
    float y;
    float z;

    float u;
    float v;
} Point;

typedef struct {
    unsigned long ip[3];
} Face;

Point* cnormal(Point a, Point b, Point c);

unsigned long hash(unsigned const char* str);

#endif