#include <cglm/cglm.h>
#include "object.h"

void object_create(Object* o, GLfloat* vertices, size_t vert_size, GLuint* indices, size_t idx_size) {
    glm_mat4_identity(o->model);


    glGenVertexArrays(1, &o->vao);
    glBindVertexArray(o->vao);

    glGenBuffers(1, &o->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, o->vbo);
    glBufferData(GL_ARRAY_BUFFER, vert_size, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);


    glGenBuffers(1, &o->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_size, indices, GL_STATIC_DRAW);
    o->n_indices = idx_size/(sizeof(GLfloat));
}

void object_draw(Object* o, GLuint model_loc) {
    glBindVertexArray(o->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o->ebo);
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, o->model[0]);

    glDrawElements(GL_TRIANGLES, o->n_indices, GL_UNSIGNED_INT, 0);
}

void object_destroy(Object* o) {
    glDeleteBuffers(1, &o->ebo);
    glDeleteBuffers(1, &o->vbo);
    glDeleteVertexArrays(1, &o->vao);
}
