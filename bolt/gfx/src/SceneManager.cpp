#include "gfx/SceneManager.hpp"

#include "util/common.h"

namespace bolt {
namespace gfx {

SceneManager::SceneManager() : mSceneRoot(math::Matrix44f::IDENTITY) {

}

SceneManager::~SceneManager() {
    for (Camera* camera : mCameras) {
        delete camera;
    }
    for (Drawable3d* drawable : mDrawables) {
        delete drawable;
    }
}

OrbitCamera* SceneManager::createOrbitCamera() {
    auto* cam = new OrbitCamera;
    mCameras.push_back(cam);
    return cam;
}

void SceneManager::loadAll() {
    for (Drawable3d* drawable : mDrawables) {
        RUNTIME_ASSERT(drawable != nullptr, "null drawable");
        drawable->load();

        // Bind uniform of camera projection and view matrices to 0
        // TODO: Can drawables share shaders?
        Shader& shader = drawable->shader();
        shader.use();
        unsigned int uniformIndex = glGetUniformBlockIndex(shader.id(), "Matrices");
        glUniformBlockBinding(shader.id(), uniformIndex, 0);
        glCheckError();
    }
}

void SceneManager::draw(Camera* camera) {
    mSceneRoot.updateWorldTransforms();
    camera->onDraw();
    for (const auto& drawable : mDrawables) {
        RUNTIME_ASSERT(drawable != nullptr, "null drawable");
        drawable->shader().use();
        drawable->shader().setMat4("model", drawable->mtx());
        drawable->draw();
    }
}

void SceneManager::draw() {
    for (Camera* camera : mCameras) {
        draw(camera);
    }
}

} // namespace gfx
} // namespace bolt
