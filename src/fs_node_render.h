#pragma once
#ifndef FS_LIB_NODE_RENDER
#define FS_LIB_NODE_RENDER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glut.h>
#include "datatype/fs_node.h"
#include "datatype/fs_list.h"
#include "loader/3d_obj.h"

typedef struct _fs_3d_data {
    GLuint model;  // glList
    GLuint tex_diffuse;  // glTexture, we will support only one texture.
    GLuint tex_normal;  // glTexture, normal texture.

    float collision_radius_2d;

    float pos[3];
    float scale[3];
    float rotate[3];

    bool (*collision_check)(struct _fs_3d_data*, Point, float);
} fs_3d_data;

node_head render;  // Render Scene Tree

void renderNodeIn(node* target);
void renderNodeOut(node* target);

node* fs_renderNode_appendChild(node* parent);
node* fs_renderNode_setOBJ(node* target, char* filename);

int fs_renderNode_render(node* target);


bool mapCollideCheck(node* _no_use, Point player, float player_radius);
bool sphericalCollideCheck(node* target, Point player, float player_radius);

bool fs_renderNode_isCollide(node* target, Point player, float player_radius);

#endif