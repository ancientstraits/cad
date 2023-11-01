#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <epoxy/gl.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "error.h"
#include "shader_util.h"

static GLfloat vertices[] = {
	0.5, 0.5, 1.0,
	0.5, -0.5, 1.0,
	-0.5, -0.5, 0.0,
	-0.5, 0.5, 0.0,
	//0.0, -0.5, 2.0,
};

static GLuint indices[] = {
	0, 1, 2,
	0, 2, 3,
};

const char* vert_shader =
	"#version 330 core\n"

	"layout (location = 0) in vec3 vp;"
	"uniform mat4 view, projection;"

	"void main() {"
		"gl_Position = vec4(vp, 1.0) * view * projection;"
	"}"
;
const char* frag_shader =
	"#version 330 core\n"

	"out vec4 frag_color;"

	"void main() {"
		"frag_color = vec4(1.0, 1.0, 1.0, 1.0);"
	"}"
;

static GLuint make_ebo(GLuint* data, GLint n_indices) {
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*n_indices, data, GL_STATIC_DRAW);

	return ebo;
}
static GLuint make_vao(GLfloat* data, GLint n_verts) {
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*n_verts, data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	return vao;
}

float zx_angle = 0.0;
float y_angle = 0.0;
static void on_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_LEFT)  zx_angle -= 0.01;
		if (key == GLFW_KEY_RIGHT) zx_angle += 0.01;

		if (key == GLFW_KEY_UP)    y_angle -= 0.01;
		if (key == GLFW_KEY_DOWN)  y_angle += 0.01;
	}
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

	GLFWwindow *window = glfwCreateWindow(600, 400, "Tutorial 01", NULL, NULL);
	ASSERT(window, "Failed to created window");
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, on_key);

	GLuint shader = shader_from_strings(vert_shader, frag_shader);
	GLuint proj_loc = glGetUniformLocation(shader, "projection");
	GLuint view_loc = glGetUniformLocation(shader, "view");
	GLuint vao = make_vao(vertices, 4);
	GLuint ebo = make_ebo(indices, 6);

	mat4 projection, view;
	float aspect = 600.0/400.0;
	glm_mat4_identity(projection);
	glm_ortho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0, projection);

	/*
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f ", projection[i][j]);
		}
		printf("\n");
	}
	printf("\n");

	vec4 test;
	glm_mat4_mulv(projection, (vec4){0.5, 0.5, 5.0, 1.0}, test);
	printf("(%f, %f, %f, %f)\n", test[0], test[1], test[2], test[3]);
	*/

	vec3 center = {0.0, 0.0, 0.0}, up = {0.0, 1.0, 0.0};

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0, 0.3, 0.3, 0.0);

		glUseProgram(shader);

		printf("%f\n", zx_angle);
		vec3 eye = {0.0, 0.0, -1.0};
		glm_lookat(eye, center, up, view);
		glm_rotate(view, zx_angle, (vec3){0.0, 1.0, 0.0});

		glUniformMatrix4fv(proj_loc, 1, GL_TRUE, projection[0]);
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, view[0]);

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

