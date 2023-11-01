#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#include "scene.h"
#include "shader_util.h"

const char* vert_code =
	"#version 330 core\n"

	"layout (location = 0) in vec3 pos;"
	"layout (location = 1) in vec3 norm;"
	"uniform mat4 model, view, projection;"

	"out vec4 fNorm;"

	"void main() {"
		"fNorm = normalize(vec4(norm, 1.0));"
		"gl_Position = projection * view * model * vec4(pos, 1.0);"
	"}"
;
const char* frag_code = 
	"#version 330 core\n"

	"const vec4 lightDir = vec4(0.0, 0.0, 1.0, 0.0);"

	"in vec4 fNorm;"
	"out vec4 frag_color;"

	"void main() {"
		"float diffuse = (dot(fNorm, lightDir) + 1.0) * 0.5;"
		"frag_color = vec4(diffuse, diffuse, diffuse, 1.0);"
	"}"
;


void scene_create(Scene* s, float aspect) {
    s->objs = NULL;
    s->prog = shader_from_strings(vert_code, frag_code);
    glm_mat4_identity(s->view);
    glm_ortho(-aspect, aspect, -1.0, 1.0, -10.0, 10.0, s->projection);
    s->model_loc       = glGetUniformLocation(s->prog, "model");
    s->view_loc        = glGetUniformLocation(s->prog, "view");
    s->projection_loc  = glGetUniformLocation(s->prog, "projection");
}
void scene_draw(Scene *s) {
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(s->prog);

    // TODO why GL_TRUE????
    glUniformMatrix4fv(s->projection_loc, 1, GL_TRUE,  s->projection[0]);
    glUniformMatrix4fv(s->view_loc,       1, GL_FALSE, s->view[0]);

    for (size_t i = 0; i < arrlen(s->objs); i++) {
        object_draw(&s->objs[i], s->model_loc);
    }
}
void scene_add_object(Scene* s, Object o) {
    arrput(s->objs, o);
}
void scene_del_object(Scene *s, size_t idx) {
    object_destroy(&s->objs[idx]);
    arrdel(s->objs, idx);
}
void scene_destroy(Scene *s) {
    for (size_t i = 0; i < arrlen(s->objs); i++) {
        object_destroy(&s->objs[i]);
    }
}

