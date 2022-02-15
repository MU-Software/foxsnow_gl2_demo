#pragma once
#ifndef FS_LIB_LOADER_WAVEFRONT_OBJ
#define FS_LIB_LOADER_WAVEFRONT_OBJ

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <GL/glut.h>

#include "../datatype/fs_list.h"
#include "../datatype/fs_rbtree.h"
#include "../utils/fs_stdfunc.h"

#define CACHE_STRING_SIZE 128

typedef struct _FS_LoaderOBJ_DataContainer {
	float* vb_vertex;
	float* vb_normal;
	float* vb_texcoord;
	int* element_buffer;
	int vb_vertex_size, vb_normal_size, vb_texcoord_size;
	int element_buffer_size;

	float collision_radius;

	list* texture_list;
} FS_LoaderOBJ_DataContainer;

typedef struct _FS_VertexCacheData {
	char cache_string[CACHE_STRING_SIZE];
	int index;
} FS_VCData;

typedef struct _FS_ElementAttribIndexes {
	int pos;
	int uv;
	int normal;

	char cache_string[CACHE_STRING_SIZE];
	unsigned long cache_hash;
} FS_EAIndex;

typedef struct _FS_FaceElementIndexes {
	FS_EAIndex p[3];
	GLuint tex_id;
} FS_FEIndex;

// TOO BAD, DO NOT DESIGN CODE LIKE THIS!
extern float max_obj_radius;

GLuint loadOBJ(const char* filename);

#endif