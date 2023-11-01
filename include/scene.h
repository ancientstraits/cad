#pragma once

#include "object.h"

typedef struct Scene {
    GLuint prog;
    mat4 view, projection;
    GLuint model_loc, view_loc, projection_loc;
    Object* objs;
} Scene;

void scene_create(Scene* s, float aspect);
void scene_add_object(Scene* s, Object o); // copies `o` to `objs`
void scene_del_object(Scene* s, size_t idx);
void scene_draw(Scene* s);
void scene_destroy(Scene* s);
