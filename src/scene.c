#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "scene.h"
#include "shader_util.h"
#include "error.h"

const char* vert_code =
	"#version 330 core\n"

	"layout (location = 0) in vec3 pos;"
	"layout (location = 1) in vec3 norm;"
	"uniform mat4 model, view, projection;"
    "uniform float currentObject;"

    "out float fCurrentObject;"
	"out vec4 fNorm;"

	"void main() {"
        "fCurrentObject = currentObject;"
		"fNorm = normalize(model * vec4(norm, 1.0));"
		"gl_Position = projection * view * model * vec4(pos, 1.0);"
	"}"
;
const char* frag_code = 
	"#version 330 core\n"

	"const vec4 lightDir = vec4(0.0, 0.0, 1.0, 0.0);"

    "in float fCurrentObject;"
	"in vec4 fNorm;"
	"out vec4 frag_color;"

	"void main() {"
		"float diffuse = (dot(fNorm, lightDir) + 1.0) * 0.5;"
        "vec3 color = fCurrentObject * vec3(1.0, 0.5, 0.0) + (1.0 - fCurrentObject) * vec3(1.0);"
		"frag_color = vec4(color*diffuse, 1.0);"
	"}"
;
const char* click_vert_code =
	"#version 330 core\n"

	"layout (location = 0) in vec3 pos;"
	"layout (location = 1) in vec3 norm;"
	"uniform mat4 model, view, projection;"
    "uniform float id;"

	"out vec4 fNorm;"
    "out float fId;"

	"void main() {"
		"fNorm = normalize(vec4(norm, 1.0));"
        "fId = id;"
		"gl_Position = projection * view * model * vec4(pos, 1.0);"
	"}"
;
const char* click_frag_code = 
	"#version 330 core\n"

    "in float fId;"
    "out vec4 outId;"

	"void main() {"
        "outId = vec4(fId, 0.0, 0.0, 0.0);"
	"}"
;

void scene_create(Scene* s, float aspect) {
    s->cur_obj = -1;
    s->objs = NULL;
    s->prog = shader_from_strings(vert_code, frag_code);
    s->click_prog = shader_from_strings(click_vert_code, click_frag_code);
    glm_mat4_identity(s->view);
    glm_ortho(-aspect, aspect, -1.0, 1.0, -10.0, 10.0, s->projection);
    s->model_loc       = glGetUniformLocation(s->prog, "model");
    s->view_loc        = glGetUniformLocation(s->prog, "view");
    s->projection_loc  = glGetUniformLocation(s->prog, "projection");
    s->cur_obj_loc     = glGetUniformLocation(s->prog, "currentObject");
    s->click_model_loc       = glGetUniformLocation(s->click_prog, "model");
    s->click_view_loc        = glGetUniformLocation(s->click_prog, "view");
    s->click_projection_loc  = glGetUniformLocation(s->click_prog, "projection");
    s->click_id_loc          = glGetUniformLocation(s->click_prog, "id");


	glGenFramebuffers(1, &s->click_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, s->click_fbo);
	glGenTextures(1, &s->click_tex);
	glBindTexture(GL_TEXTURE_2D, s->click_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 600, 400, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s->click_tex, 0);
	ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer incomplete");
    

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
static void scene_draw_with_prog(Scene* s, GLuint prog) {
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(prog);
    GL();

    glUniformMatrix4fv(s->projection_loc, 1, GL_FALSE, s->projection[0]);
    GL();
    glUniformMatrix4fv(s->view_loc,       1, GL_FALSE, s->view[0]);
    GL();

    for (size_t i = 0; i < arrlen(s->objs); i++) {
        glUniform1f(s->cur_obj_loc, (i == s->cur_obj) ? 1.0 : 0.0);
        object_draw(&s->objs[i], s->model_loc);
    }
}
void scene_draw(Scene *s) {
    scene_draw_with_prog(s, s->prog);
}
int scene_get_object_at(Scene *s, int x, int y) {
    glBindFramebuffer(GL_FRAMEBUFFER, s->click_fbo);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(s->click_prog);
    GL();

    // how does this work?!
    glUniformMatrix4fv(s->click_projection_loc, 1, GL_FALSE, s->projection[0]);
    GL();
    glUniformMatrix4fv(s->click_view_loc,       1, GL_FALSE, s->view[0]);
    GL();

    for (size_t i = 0; i < arrlen(s->objs); i++) {
        glUniform1f(s->click_id_loc, ((float)(i+1))/256.0); GL();
        object_draw(&s->objs[i], s->click_model_loc);
    }


    uint8_t px;
    glReadPixels(x, y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &px); GL();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return (int)px - 1;
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

