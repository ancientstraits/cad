#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <epoxy/gl.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "error.h"
#include "object.h"
#include "primitive.h"
#include "scene.h"

/*
const char* frag_shader =
	"#version 330 core\n"

	"out int val;"

	"void main() {"
		"val = 5;"
	"}"
;
*/

float zx_angle = M_PI/4;
float y_angle = M_PI/4;
int move_mouse = 0;
double mouse_pos[2] = {0, 0};
enum {
	EM_NONE,
	EM_MOVE_X,
	EM_MOVE_Y,
	EM_MOVE_Z,
	EM_SCALE_X,
	EM_SCALE_Y,
	EM_SCALE_Z,
	EM_ROT_X,
	EM_ROT_Y,
	EM_ROT_Z,
} edit_mode;
Scene s;

static void on_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS   && key == GLFW_KEY_LEFT_SHIFT) {
		move_mouse = 1;
		glfwGetCursorPos(window, &(mouse_pos[0]), &(mouse_pos[1]));
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_LEFT_SHIFT) move_mouse = 0;
	if (action == GLFW_PRESS && s.cur_obj != -1) {
		if (key == GLFW_KEY_R) scene_del_object(&s, s.cur_obj);
		if (key == GLFW_KEY_Q) edit_mode = EM_MOVE_X;
		if (key == GLFW_KEY_W) edit_mode = EM_MOVE_Y;
		if (key == GLFW_KEY_E) edit_mode = EM_MOVE_Z;
		if (key == GLFW_KEY_A) edit_mode = EM_SCALE_X;
		if (key == GLFW_KEY_S) edit_mode = EM_SCALE_Y;
		if (key == GLFW_KEY_D) edit_mode = EM_SCALE_Z;
		if (key == GLFW_KEY_Z) edit_mode = EM_ROT_X;
		if (key == GLFW_KEY_X) edit_mode = EM_ROT_Y;
		if (key == GLFW_KEY_C) edit_mode = EM_ROT_Z;
		glfwGetCursorPos(window, &(mouse_pos[0]), &(mouse_pos[1]));
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_N) {
		Object o;
		box_create_centered(&o, (vec3){0, 0, 0}, (vec3){0.1, 0.1, 0.1});
		scene_add_object(&s, o);
	}
}
static void on_click(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
		if (edit_mode != EM_NONE) {
			edit_mode = EM_NONE;
		} else {
			double pos[2];
			glfwGetCursorPos(window, &pos[0], &pos[1]);
			s.cur_obj = scene_get_object_at(&s, (int)pos[0], (int)pos[1]);
		}
	}
}


int main() {
	ASSERT(glfwInit(), "Could not initialize GLFW");

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(600, 400, "MTF", NULL, NULL);
	ASSERT(window, "Failed to created window");
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, on_key);
	glfwSetMouseButtonCallback(window, on_click);

	glEnable(GL_DEPTH_TEST);

	Object o;
	box_create_centered(&o, (vec3){0.0, 0.0, 0.0}, (vec3){.1, .1, .1});

	scene_create(&s, 600.0/400.0);
	scene_add_object(&s, o);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1, 1, 0, GL_RED, GL_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer incomplete");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	while (!glfwWindowShouldClose(window)) {
		if (move_mouse) {
			const double mouse_mul = 0.01;
			double new_pos[2];
			glfwGetCursorPos(window, &(new_pos[0]), &(new_pos[1]));
			zx_angle += mouse_mul * (new_pos[0] - mouse_pos[0]);
			y_angle  += mouse_mul * (new_pos[1] - mouse_pos[1]);

			y_angle = glm_clamp(y_angle, -M_PI/2, M_PI/2);

			mouse_pos[0] = new_pos[0];
			mouse_pos[1] = new_pos[1];
		}
		if (edit_mode == EM_MOVE_X || edit_mode == EM_MOVE_Y || edit_mode == EM_MOVE_Z) {
			double new_pos[2];
			glfwGetCursorPos(window, &(new_pos[0]), &(new_pos[1]));
			double mul = 0.005;
			double dx = mul*(new_pos[0] - mouse_pos[0]);

			vec3 dpos = {
				// remap axes to what I want
				-dx*(edit_mode == EM_MOVE_Y),
				dx*(edit_mode == EM_MOVE_Z),
				-dx*(edit_mode == EM_MOVE_X),
			};
			glm_translate(s.objs[s.cur_obj].model, dpos);

			mouse_pos[0] = new_pos[0];
			mouse_pos[1] = new_pos[1];
		}
		if (edit_mode == EM_SCALE_X || edit_mode == EM_SCALE_Y || edit_mode == EM_SCALE_Z) {
			double new_pos[2];
			glfwGetCursorPos(window, &(new_pos[0]), &(new_pos[1]));
			double mul = 0.05;
			double dx = mul*(new_pos[0] - mouse_pos[0]);

			vec3 dpos = {
				// remap axes to what I want
				dx*(edit_mode == EM_SCALE_Y) + 1.0,
				dx*(edit_mode == EM_SCALE_Z)  + 1.0,
				dx*(edit_mode == EM_SCALE_X) + 1.0,
			};
			glm_scale(s.objs[s.cur_obj].model, dpos);

			mouse_pos[0] = new_pos[0];
			mouse_pos[1] = new_pos[1];
		}
		if (edit_mode == EM_ROT_X || edit_mode == EM_ROT_Y || edit_mode == EM_ROT_Z) {
			double new_pos[2];
			glfwGetCursorPos(window, &(new_pos[0]), &(new_pos[1]));
			double mul = 0.1;
			double dx = mul*(new_pos[0] - mouse_pos[0]);

			vec3 axis = {
				edit_mode == EM_ROT_Y,
				edit_mode == EM_ROT_Z,
				edit_mode == EM_ROT_X,
			};
			glm_rotate(s.objs[s.cur_obj].model, dx, axis);

			mouse_pos[0] = new_pos[0];
			mouse_pos[1] = new_pos[1];
		}


		vec3 eye = {0.0, 0.0, -1.0}, center = {0.0, 0.0, 0.0}, up = {0.0, 1.0, 0.0};
		glm_lookat(eye, center, up, s.view);
		glm_rotate(s.view, y_angle, (vec3){-1.0, 0.0, 0.0});
		glm_rotate(s.view, zx_angle, (vec3){0.0, 1.0, 0.0});


		scene_draw(&s);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	scene_destroy(&s);
	glfwTerminate();
	return 0;
}

