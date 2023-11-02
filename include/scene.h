#pragma once

#include "object.h"

typedef struct Scene {
    GLuint prog;
    mat4 view, projection;
    GLuint model_loc, view_loc, projection_loc, cur_obj_loc;
    GLuint click_model_loc, click_view_loc, click_projection_loc;
    GLuint click_prog, click_id_loc, click_fbo, click_tex;
    Object* objs;
    int cur_obj;
} Scene;

void scene_create(Scene* s, float aspect);
void scene_add_object(Scene* s, Object o); // copies `o` to `objs`
void scene_del_object(Scene* s, size_t idx);
int scene_get_object_at(Scene* s, int x, int y);
void scene_draw(Scene* s);
void scene_destroy(Scene* s);
