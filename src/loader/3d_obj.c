#include "3d_obj.h"

GLuint image_data = 0;
float* temp_vertex = NULL;
float* temp_texcoord = NULL;
float max_obj_radius = 0.0f;

FS_VCData* find_eaindex_from_list(list* src, const char* target) {
    list_element* current_element = src->head;
    while (current_element) {
        FS_VCData* current_element_data = (FS_VCData*)(current_element->data);
        if (!strcmp(current_element_data->cache_string, target)) return current_element_data;

        current_element = current_element->next;
    }
    return NULL;
}

void commit_point_cache_in_face(FS_FEIndex* face) {
    if (!face) return;
    int z = 0;
    for (z; z < 3; z++) {
        FS_EAIndex* point = &(face->p[z]);
        sprintf(point->cache_string, "%d/%d/%d", point->pos, point->uv, point->normal);
        point->cache_hash = hash(point->cache_string);
    }
}

float* list_to_heap_array_float(list* src, int count) {
    if (!src || !src->length) return NULL;
    float* result_array = (float*)calloc((src->length) * count, sizeof(float));
    if (!result_array) return NULL;

    int index = 0;
    list_element* target_element = src->head;
    while (target_element) {
        memcpy((void*)(result_array + (index * count)), target_element->data, sizeof(float) * count);
        index++;
        target_element = target_element->next;
    }
    return result_array;
}

GLuint loadOBJ(const char* filename) {
    FILE* file_pointer = fopen(filename, "r");
    if (file_pointer == NULL) {
        printf("OBJ_LOADER > OBJ file load Failed.\n");
        return 0;
    }

    if (temp_vertex) free(temp_vertex);
    if (temp_texcoord) free(temp_texcoord);
    image_data = 0;

    list* list_vertex = create_list();  // free checked
    list* list_texcoord = create_list();  // free checked
    list* list_normal = create_list();  // free checked

    list* obj_index = create_list();  // free checked

    // obj_type[0] => 't'ri or 'q'uad
    // obj_type[1] => 0b00 - 0b11, 0b[uv][norm]
    char obj_type[2] = { 0 };
    int face_count = 0;

    while (!feof(file_pointer)) {
        // Read one line and remove \n character.
        char str_buf[512] = { 0 };
        char* _tmp = NULL;
        fgets(str_buf, (int)sizeof(str_buf), file_pointer);
        _tmp = strtok(str_buf, "\n");
        if (!(strcmp(str_buf, ""))) continue;

        int num_read = 0;
        char prefix[3] = { 0 };
        float x, y, z;
        if ((num_read = sscanf(str_buf, "%2s %f %f %f", prefix, &x, &y, &z)) < 1) continue;

        switch (prefix[0]) {
        case 'v': {  // Vertex attributes parser
            switch (prefix[1]) {
            case '\0':  // Vertex position
                if (num_read != 4) continue;
                float* tmp_vert = (float*)calloc(3, sizeof(float));
                tmp_vert[0] = x, tmp_vert[1] = y, tmp_vert[2] = z;
                //float current_radius = sqrt(x * x + y * y + z * z);
                float current_radius = sqrt(x * x + z * z);  // Only check XZ plane
                if (current_radius > max_obj_radius) max_obj_radius = current_radius;
                list_append(list_vertex, tmp_vert, 0);
                break;
            case 't':  // Texture coordinate
                if (num_read != 3) continue;
                float* tmp_uv = (float*)calloc(2, sizeof(float));
                tmp_uv[0] = x, tmp_uv[1] = y;
                list_append(list_texcoord, tmp_uv, 0);
                break;
            case 'n':  // Normal vector
                if (num_read != 4) continue;
                float* tmp_norm = (float*)calloc(3, sizeof(float));
                tmp_norm[0] = x, tmp_norm[1] = y, tmp_norm[2] = z;
                list_append(list_normal, tmp_norm, 0);
                break;
            default:
                continue;
            }
        } break;
        case 'f': {  // Face parser
            int ind_pos[4] = { 0 };
            int ind_uv[4] = { 0 };
            int ind_norm[4] = { 0 };
            face_count++;
            // Handle quads (four vertices on one face)
            if (sscanf(str_buf, "%2s %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",  // Quad with pos/uv/norm
                prefix,
                &ind_pos[0], &ind_uv[0], &ind_norm[0],
                &ind_pos[1], &ind_uv[1], &ind_norm[1],
                &ind_pos[2], &ind_uv[2], &ind_norm[2],
                &ind_pos[3], &ind_uv[3], &ind_norm[3]) == 13) {
                if (!obj_type[0]) obj_type[0] = 'q', obj_type[1] = 0b11;
                else if (obj_type[0] != 'q' || obj_type[1] != 0b11) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0], face_1->p[0].uv = ind_uv[0], face_1->p[0].normal = ind_norm[0];
                face_1->p[1].pos = ind_pos[1], face_1->p[1].uv = ind_uv[1], face_1->p[1].normal = ind_norm[1];
                face_1->p[2].pos = ind_pos[3], face_1->p[2].uv = ind_uv[3], face_1->p[2].normal = ind_norm[3];
                face_1->tex_id = image_data;
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);

                FS_FEIndex* face_2 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_2);
                face_2->p[0].pos = ind_pos[1], face_2->p[0].uv = ind_uv[1], face_2->p[0].normal = ind_norm[1];
                face_2->p[1].pos = ind_pos[2], face_2->p[1].uv = ind_uv[2], face_2->p[1].normal = ind_norm[2];
                face_2->p[2].pos = ind_pos[3], face_2->p[2].uv = ind_uv[3], face_2->p[2].normal = ind_norm[3];
                face_1->tex_id = image_data;
                commit_point_cache_in_face(face_2);
                list_append(obj_index, (void*)face_2, 0);
            }
            else if (sscanf(str_buf, "%2s %d//%d %d//%d %d//%d %d//%d",  // Quad with pos//norm
                prefix,
                &ind_pos[0], &ind_norm[0],
                &ind_pos[1], &ind_norm[1],
                &ind_pos[2], &ind_norm[2],
                &ind_pos[3], &ind_norm[3]) == 9) {
                if (!obj_type[0]) obj_type[0] = 'q', obj_type[1] = 0b01;
                else if (obj_type[0] != 'q' || obj_type[1] != 0b01) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0], face_1->p[0].normal = ind_norm[0];
                face_1->p[1].pos = ind_pos[1], face_1->p[1].normal = ind_norm[1];
                face_1->p[2].pos = ind_pos[3], face_1->p[2].normal = ind_norm[3];
                face_1->tex_id = image_data;
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);

                FS_FEIndex* face_2 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_2);
                face_2->p[0].pos = ind_pos[1], face_2->p[0].normal = ind_norm[1];
                face_2->p[1].pos = ind_pos[2], face_2->p[1].normal = ind_norm[2];
                face_2->p[2].pos = ind_pos[3], face_2->p[2].normal = ind_norm[3];
                face_1->tex_id = image_data;
                commit_point_cache_in_face(face_2);
                list_append(obj_index, (void*)face_2, 0);
            }
            else if (sscanf(str_buf, "%2s %d/%d %d/%d %d/%d %d/%d",  // Quad with pos/uv
                prefix,
                &ind_pos[0], &ind_uv[0],
                &ind_pos[1], &ind_uv[1],
                &ind_pos[2], &ind_uv[2],
                &ind_pos[3], &ind_uv[3]) == 9) {
                if (!obj_type[0]) obj_type[0] = 'q', obj_type[1] = 0b10;
                else if (obj_type[0] != 'q' || obj_type[1] != 0b10) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0], face_1->p[0].uv = ind_uv[0];
                face_1->p[1].pos = ind_pos[1], face_1->p[1].uv = ind_uv[1];
                face_1->p[2].pos = ind_pos[3], face_1->p[2].uv = ind_uv[3];
                face_1->tex_id = image_data;
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);

                FS_FEIndex* face_2 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_2);
                face_2->p[0].pos = ind_pos[1], face_2->p[0].uv = ind_uv[1];
                face_2->p[1].pos = ind_pos[2], face_2->p[1].uv = ind_uv[2];
                face_2->p[2].pos = ind_pos[3], face_2->p[2].uv = ind_uv[3];
                face_1->tex_id = image_data;
                commit_point_cache_in_face(face_2);
                list_append(obj_index, (void*)face_2, 0);
            }
            else if (sscanf(str_buf, "%2s %d %d %d %d",  // Quad with pos only
                prefix, &ind_pos[0], &ind_pos[1], &ind_pos[2], &ind_pos[3]) == 5) {
                if (!obj_type[0]) obj_type[0] = 'q', obj_type[1] = 0b00;
                else if (obj_type[0] != 'q' || obj_type[1] != 0b00) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0];
                face_1->p[1].pos = ind_pos[1];
                face_1->p[2].pos = ind_pos[3];
                face_1->tex_id = image_data;
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);

                FS_FEIndex* face_2 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_2);
                face_2->p[0].pos = ind_pos[1];
                face_2->p[1].pos = ind_pos[2];
                face_2->p[2].pos = ind_pos[3];
                face_1->tex_id = image_data;
                commit_point_cache_in_face(face_2);
                list_append(obj_index, (void*)face_2, 0);
            }


            // Handle triangles, this is normal. (four vertices on one face)
            else if (sscanf(str_buf, "%2s %d/%d/%d %d/%d/%d %d/%d/%d",  // Tri with pos/uv/norm
                prefix,
                &ind_pos[0], &ind_uv[0], &ind_norm[0],
                &ind_pos[1], &ind_uv[1], &ind_norm[1],
                &ind_pos[2], &ind_uv[2], &ind_norm[2]) == 10) {
                if (!obj_type[0]) obj_type[0] = 't', obj_type[1] = 0b11;
                else if (obj_type[0] != 't' || obj_type[1] != 0b11) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0], face_1->p[0].uv = ind_uv[0], face_1->p[0].normal = ind_norm[0];
                face_1->p[1].pos = ind_pos[1], face_1->p[1].uv = ind_uv[1], face_1->p[1].normal = ind_norm[1];
                face_1->p[2].pos = ind_pos[2], face_1->p[2].uv = ind_uv[2], face_1->p[2].normal = ind_norm[2];
                face_1->tex_id = image_data;
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);
            }
            else if (sscanf(str_buf, "%2s %d//%d %d//%d %d//%d",  // Tri with pos//norm
                prefix,
                &ind_pos[0], &ind_norm[0],
                &ind_pos[1], &ind_norm[1],
                &ind_pos[2], &ind_norm[2]) == 7) {
                if (!obj_type[0]) obj_type[0] = 't', obj_type[1] = 0b01;
                else if (obj_type[0] != 't' || obj_type[1] != 0b01) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0], face_1->p[0].normal = ind_norm[0];
                face_1->p[1].pos = ind_pos[1], face_1->p[1].normal = ind_norm[1];
                face_1->p[2].pos = ind_pos[2], face_1->p[2].normal = ind_norm[2];
                face_1->tex_id = image_data;
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);
            }
            else if (sscanf(str_buf, "%2s %d/%d %d/%d %d/%d",  // Tri with pos/uv
                prefix,
                &ind_pos[0], &ind_uv[0],
                &ind_pos[1], &ind_uv[1],
                &ind_pos[2], &ind_uv[2]) == 7) {
                if (!obj_type[0]) obj_type[0] = 't', obj_type[1] = 0b10;
                else if (obj_type[0] != 't' || obj_type[1] != 0b10) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0], face_1->p[0].uv = ind_uv[0];
                face_1->p[1].pos = ind_pos[1], face_1->p[1].uv = ind_uv[1];
                face_1->p[2].pos = ind_pos[2], face_1->p[2].uv = ind_uv[2];
                face_1->tex_id = image_data;
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);
            }
            else if (sscanf(str_buf, "%2s %d %d %d",  // Tri with pos only
                prefix, &ind_pos[0], &ind_pos[1], &ind_pos[2]) == 4) {
                if (!obj_type[0]) obj_type[0] = 't', obj_type[1] = 0b00;
                else if (obj_type[0] != 't' || obj_type[1] != 0b00) goto error_after_list_creation;

                FS_FEIndex* face_1 = (FS_FEIndex*)calloc(1, sizeof(FS_FEIndex));
                ALLOC_FAILCHECK(face_1);
                face_1->p[0].pos = ind_pos[0];
                face_1->p[1].pos = ind_pos[1];
                face_1->p[2].pos = ind_pos[2];
                face_1->tex_id = image_data;
                commit_point_cache_in_face(face_1);
                list_append(obj_index, (void*)face_1, 0);
            }
        } break;
        case 't': {  // Custom texture load command
            char tmp_buf_1[512] = { 0 };
            char prefix[3] = { 0 };
            if (num_read = sscanf(str_buf, "%2s %s", prefix, tmp_buf_1) == 2) {
                GLuint texture_load_result = loadBMP(tmp_buf_1);
                if (texture_load_result) {
                    glEnable(GL_TEXTURE_2D);
                    image_data = texture_load_result;
                    printf("Texture <%s> loaded!\n", tmp_buf_1);
                }
                else {
                    printf("Texture load failed!\n");
                }
            }
        } break;
        case '#':  // Comment
            continue;
        default:
            continue;
        }
    }
    fclose(file_pointer);

    if (list_vertex->length == 0)  // list_vertex must not be empty.
        goto error_after_list_creation;

    // Now let's convert list to array for fast random accessing.
    float* arr_vertex = list_to_heap_array_float(list_vertex, 3);
    float* arr_texcoord = list_to_heap_array_float(list_texcoord, 2);
    float* arr_normal = list_to_heap_array_float(list_normal, 3);

    // Port from Foxsnow(Personal project) : This was modified to support OpenGL2
    // Generate GL command list and use it.
    GLuint new_list = glGenLists(1);
    glNewList(new_list, GL_COMPILE);

    GLuint last_binded_tex = 0;
    glBindTexture(GL_TEXTURE_2D, 0);

    list_element* target_process_face_element = obj_index->head;
    while (target_process_face_element) {  // face processing
        int vertex_index = 0;
        FS_FEIndex* face = (FS_FEIndex*)target_process_face_element->data;

        if (last_binded_tex != face->tex_id) {
            glBindTexture(GL_TEXTURE_2D, face->tex_id);
            last_binded_tex = face->tex_id;
        }

        Point* vtx_point = (Point*)calloc(3, sizeof(Point));
        ALLOC_FAILCHECK(vtx_point);
        int vertex_point_pos = 0;
        for (vertex_point_pos; vertex_point_pos < 3; vertex_point_pos++) {  // triangle point processing
            FS_EAIndex point = face->p[vertex_point_pos];

            int vp_pointer = (point.pos - 1) * 3;  // Same as normal pointer
            int uv_pointer = (point.uv - 1) * 2;

            (vtx_point+vertex_point_pos)->x = (arr_vertex + vp_pointer)[0];
            (vtx_point+vertex_point_pos)->y = (arr_vertex + vp_pointer)[1];
            (vtx_point+vertex_point_pos)->z = (arr_vertex + vp_pointer)[2];

            (vtx_point+vertex_point_pos)->u = (arr_texcoord + uv_pointer)[0];
            (vtx_point+vertex_point_pos)->v = (arr_texcoord + uv_pointer)[1];
        }
        Point* norm = cnormal(vtx_point[0], vtx_point[1], vtx_point[2]);


        glBegin(GL_TRIANGLES);

        glNormal3f(norm->x, norm->y, norm->z);
        // Below will be zero when there's no texcoord data.
        // That's why I prefer calloc than malloc
        glTexCoord2f(vtx_point[0].u, vtx_point[0].v);
        glVertex3f(vtx_point[0].x, vtx_point[0].y, vtx_point[0].z);
        glTexCoord2f(vtx_point[1].u, vtx_point[1].v);
        glVertex3f(vtx_point[1].x, vtx_point[1].y, vtx_point[1].z);
        glTexCoord2f(vtx_point[2].u, vtx_point[2].v);
        glVertex3f(vtx_point[2].x, vtx_point[2].y, vtx_point[2].z);

        glEnd();

        free(vtx_point);
        target_process_face_element = target_process_face_element->next;
    }
    glEndList();

    free_list(&list_vertex);
    free_list(&list_normal);
    free_list(&list_texcoord);
    free_list(&obj_index);

    //free(arr_vertex);
    //free(arr_normal);
    //free(arr_texcoord);

    printf("Custom OBJ model <%s> loaded: face count=%d\n", filename, face_count);

    return new_list;

error_after_list_creation:
    return 0;
}
