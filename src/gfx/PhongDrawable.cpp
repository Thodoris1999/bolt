#include "PhongDrawable.hpp"

#include "gl_defines.h"
#include "GlUtils.hpp"

PhongDrawable::PhongDrawable() {
    GlUtils::loadShader("shader.vert", vertexShader, GL_VERTEX_SHADER);
    GlUtils::loadShader("shader.frag", fragmentShader, GL_FRAGMENT_SHADER);

    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, vertexShader);
    glAttachShader(mShaderProgram, fragmentShader);
    glLinkProgram(mShaderProgram);
    GlUtils::assertProgramOk(mShaderProgram);
    glCheckError();

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void PhongDrawable::onDraw() {
    this->Drawable3d::onDraw();

    glUseProgram(mShaderProgram);
    int ambientLoc = glGetUniformLocation(mShaderProgram, "ambientColor");
    glUniform4fv(ambientLoc, 1, (const GLfloat*)&mAmbient);
}
