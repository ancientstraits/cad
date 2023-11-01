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

float zx_angle = 0.0;
float y_angle = 0.0;
int move_mouse = 0;
double mouse_pos[2] = {0, 0};
static void on_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS   && key == GLFW_KEY_LEFT_SHIFT) {
		move_mouse = 1;
		glfwGetCursorPos(window, &(mouse_pos[0]), &(mouse_pos[1]));
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_LEFT_SHIFT) move_mouse = 0;
}

int main() {
	ASSERT(glfwInit(), "Could not initialize GLFW");

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(600, 400, "evoCADive", NULL, NULL);
	ASSERT(window, "Failed to created window");
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, on_key);

	glEnable(GL_DEPTH_TEST);

	Object o;
	box_create_centered(&o, (vec3){0.0, 0.0, 0.0}, (vec3){.1, .2, .3});

	Scene s;
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

	mat4 projection, view;
	float aspect = 600.0/400.0; // 1.5
	glm_ortho(-aspect, aspect, -1.0, 1.0, -10.0, 10.0, projection);

	vec4 test;
	glm_mat4_mulv(projection, (vec4){0.5, 0.5, 1.0, 1.0}, test);

	vec3 center = {0.0, 0.0, 0.0}, up = {0.0, 1.0, 0.0};

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

		vec3 eye = {0.0, 0.0, -1.0};
		glm_lookat(eye, center, up, s.view);
		glm_rotate(s.view, y_angle, (vec3){-1.0, 0.0, 0.0});
		glm_rotate(s.view, zx_angle, (vec3){0.0, 1.0, 0.0});

		scene_draw(&s);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	uint8_t px[3];
	uint8_t val;
	glReadPixels(0, 0, 1, 1, GL_RED, GL_INT, &val);
	printf("%d\n", val);

	scene_destroy(&s);
	glfwTerminate();
	return 0;
}

