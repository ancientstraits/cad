#pragma once
// Rendering object

#include <epoxy/gl.h>
#include <cglm/cglm.h>

typedef struct Object {
    GLuint vao, vbo, ebo;
    mat4 model;
    size_t n_indices;
} Object;

void object_create(Object* o,
    GLfloat* vertices, size_t vert_size, GLuint* indices, size_t idx_size
);

void object_draw(Object* o, GLuint model_loc);

void object_destroy(Object* o);
