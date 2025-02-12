#include "Shader.hpp"

Shader::Shader(const char* vtxShader, const char* fragShader) {
    unsigned int vertexShader, fragmentShader;
    GlUtils::loadShader(vtxShader, vertexShader, GL_VERTEX_SHADER);
    GlUtils::loadShader(fragShader, fragmentShader, GL_FRAGMENT_SHADER);

    mId = glCreateProgram();
    glAttachShader(mId, vertexShader);
    glAttachShader(mId, fragmentShader);
    glLinkProgram(mId);
    GlUtils::assertProgramOk(mId);
    glCheckError();

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
