#ifndef SHADER_UTIL_H
#define SHADER_UTIL_H

#include <epoxy/gl.h>

GLuint shader_from_strings(const char* vert_code, const char* frag_code);

#endif // !SHADER_UTIL_H

