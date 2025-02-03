#include "DrawableManager.hpp"

#include "gl_defines.h"
#include "common.h"

Drawable3dManager::Drawable3dManager() {

}

void Drawable3dManager::init() {
    mCamera = new OrbitCamera;
}

Drawable3dManager::~Drawable3dManager() {
    delete mCamera;
}

void Drawable3dManager::draw() {
    mCamera->onDraw();
    for (const auto& drawable : mDrawables) {
        RUNTIME_ASSERT(drawable != nullptr, "null drawable");
        drawable->onDraw();
        drawable->draw();
    }
}

void Drawable3dManager::registerDrawable(Drawable3d* drawable) {
    // Bind uniform of camera projection and view matrices to 0
    int shader = drawable->shaderProgram();
    glUseProgram(shader);
    unsigned int uniformIndex = glGetUniformBlockIndex(shader, "Matrices");
    glUniformBlockBinding(shader, uniformIndex, 0);
    glCheckError();

    mDrawables.push_back(drawable);
}
