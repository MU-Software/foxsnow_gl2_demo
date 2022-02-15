#include "fs_node_render.h"

void renderNodeIn(node* target) {
	if (!target) return;
	fs_3d_data* target_data = (fs_3d_data*)(target->data);

	glPushMatrix();
	// Move object first.
	glTranslatef(target_data->pos[0], target_data->pos[1], target_data->pos[2]);
	// Then rotate as ZXY order
	glRotatef(target_data->rotate[2], 0.0f, 0.0f, 1.0f);
	glRotatef(target_data->rotate[0], 1.0f, 0.0f, 0.0f);
	glRotatef(target_data->rotate[1], 0.0f, 1.0f, 0.0f);
	// We do not glScalef here, because this will scale children too,
	// and We just want to scale this node only.
}

void renderNodeOut(node* target) {
	fs_3d_data* target_data = (fs_3d_data*)(target->data);

	// We should apply scale data here, because we didn't glScalef on renderNodeIn.
	glScalef(target_data->scale[0], target_data->scale[1], target_data->scale[2]);

	if (target_data->model) glCallList(target_data->model);
	glPopMatrix();
}



node* fs_renderNode_appendChild(node* parent) {
	fs_3d_data* new_data = (fs_3d_data*)calloc(1, sizeof(fs_3d_data));
	node* new_node = create_node(parent, renderNodeIn, renderNodeOut, new_data, NULL);

	return new_node;
}

node* fs_renderNode_setOBJ(node* target, char* filename) {
	if (!target) return NULL;

	fs_3d_data* target_data = (fs_3d_data*)target->data;
	if (!target_data) return NULL;
	target_data->scale[0] = 1.0f, target_data->scale[1] = 1.0f, target_data->scale[2] = 1.0f;

	target_data->model = loadOBJ(filename);
	target_data->collision_radius_2d = max_obj_radius;  // TOO BAD, DO NOT WRITE CODE LIKE THIS!

	return target;
}

// This will do pre-order tree traversal,
// This function executes function not only when visit child node,
// but also executes when getting out the child node.
int fs_renderNode_render(node* target) {
	if (!target) return;

	target->in(target);
	node* target_child = target->child;
	while (target_child) {
		fs_renderNode_render(target_child);
		target_child = target_child->next;
	}
	target->out(target);

	return 0;
}



bool mapCollideCheck(node* _no_use, Point player, float player_radius) {
	if (player.x > 10.0 || player.x < -10.0) return true;
	if (player.z > 42.0 || player.z < -83.0) return true;
	return false;
}

bool sphericalCollideCheck(node* target, Point player, float player_radius) {
	if (!target) return false;
	fs_3d_data* target_data = (fs_3d_data*)target->data;
	if (!target_data) return false;

	// Get distance of target and player
	// We just check 2d collide, so just use X and Z
	float x_ = pow(target_data->pos[0] - player.x, 2.0f);
	float y_ = pow(target_data->pos[2] - player.z, 2.0f);
	double dist = sqrt(x_ + y_);

	// Compare distance with sum of radii (player radius and target radius)
	if (dist > (target_data->collision_radius_2d * target_data->scale[0] + player_radius)) return false;
	return true;
}

bool fs_renderNode_isCollide(node* target, Point player, float player_radius) {
	if (!target) return false;

	fs_3d_data* target_data = (fs_3d_data*)target->data;
	if (!target_data) return false;

	// Collide detected on this node
	if ((target_data->collision_check) && (target_data->collision_check)(target, player, player_radius))
		return true;

	node* target_child = target->child;
	while (target_child) {
		// Collide detected on child node
		if (fs_renderNode_isCollide(target_child, player, player_radius))
			return true;
		target_child = target_child->next;
	}
	return false;
}