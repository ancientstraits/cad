#include "primitive.h"

static GLuint box_indices[] = {
	0, 1, 2,  0, 3, 2,
	4, 5, 6,  4, 7, 6,

	8, 9, 10,    8, 11, 10,
	12, 13, 14,  12, 15, 14,

	16, 17, 18,  16, 19, 18,
	20, 21, 22,  20, 23, 22,
};

void box_create_centered(Object* o, vec3 center, vec3 dim) {
    vec3 min = {center[0]-(dim[0]/2.0), center[1]-(dim[1]/2.0), center[2]-(dim[2]/2.0)};
    return box_create(o, min, dim);
}

void box_create(Object* o, vec3 min, vec3 dim) {
    vec3 max;
    glm_vec3_add(dim, min, max); // max = dim + min

    GLfloat vertices[144] = {
        max[0], max[1], max[2],  0.0, 0.0, 1.0,
        min[0], max[1], max[2],  0.0, 0.0, 1.0,
        min[0], min[1], max[2],  0.0, 0.0, 1.0,
        max[0], min[1], max[2],  0.0, 0.0, 1.0,

        max[0], max[1], min[2],  0.0, 0.0, -1.0,
        min[0], max[1], min[2],  0.0, 0.0, -1.0,
        min[0], min[1], min[2],  0.0, 0.0, -1.0,
        max[0], min[1], min[2],  0.0, 0.0, -1.0,

        max[0], max[1], max[2],  0.0, 1.0, 0.0,
        min[0], max[1], max[2],  0.0, 1.0, 0.0,
        min[0], max[1], min[2],  0.0, 1.0, 0.0,
        max[0], max[1], min[2],  0.0, 1.0, 0.0,

        max[0], min[1], max[2],  0.0, -1.0, 0.0,
        min[0], min[1], max[2],  0.0, -1.0, 0.0,
        min[0], min[1], min[2],  0.0, -1.0, 0.0,
        max[0], min[1], min[2],  0.0, -1.0, 0.0,

        max[0], max[1], max[2],  1.0, 0.0, 0.0,
        max[0], min[1], max[2],  1.0, 0.0, 0.0,
        max[0], min[1], min[2],  1.0, 0.0, 0.0,
        max[0], max[1], min[2],  1.0, 0.0, 0.0,

        min[0], max[1], max[2],  -1.0, 0.0, 0.0,
        min[0], min[1], max[2],  -1.0, 0.0, 0.0,
        min[0], min[1], min[2],  -1.0, 0.0, 0.0,
        min[0], max[1], min[2],  -1.0, 0.0, 0.0,
    };

    object_create(o, vertices, sizeof(vertices), box_indices, sizeof(box_indices));
}
