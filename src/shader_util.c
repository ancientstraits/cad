#include "shader_util.h"
#include "error.h"

static GLuint single_shader(GLuint type, const char* code) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);

    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (!result) {
        int err_len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &err_len);

        char err_msg[2048];
        glGetShaderInfoLog(shader, err_len, NULL, err_msg);

        DIE("Failed to compile shader %s: %s",
            (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment", err_msg);
    }

    return shader;
}

GLuint shader_from_strings(const char* vert_code, const char* frag_code) {
    GLuint vert_shader = single_shader(GL_VERTEX_SHADER, vert_code);
    GLuint frag_shader = single_shader(GL_FRAGMENT_SHADER, frag_code);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert_shader);
    glAttachShader(prog, frag_shader);
    glLinkProgram(prog);

    return prog;
}
