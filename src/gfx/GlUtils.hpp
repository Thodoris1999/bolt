#pragma once

#include "Filesystem.hpp"
#include "common.h"

#include "gl_defines.h"

#include <cstdlib>

namespace GlUtils {
inline void loadShader(const char* name, unsigned int& shader, GLenum type) {
    unsigned int size;
    void* source = Filesystem::loadResource(name, size);

    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar* const*)&source, (const GLint*)&size);
    glCompileShader(shader);

#ifndef NDEBUG
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        printf("Shader Source (%s):\n%s\n", name, static_cast<const char*>(source));

        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        RUNTIME_ASSERT(success, infoLog);
    }
#endif

    free(source);
}

inline void assertProgramOk(unsigned int program) {
#ifndef NDEBUG
    int  success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        RUNTIME_ASSERT(success, infoLog);
    }
#endif
}

void glDebugOutputCallback(GLenum source,
                            GLenum type,
                            unsigned int id,
                            GLenum severity,
                            GLsizei length,
                            const char *message,
                            const void *userParam);

}
