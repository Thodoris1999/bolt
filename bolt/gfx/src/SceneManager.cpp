#include "gfx/SceneManager.hpp"

#include "gfx/opengl/OpenglRenderSystem.hpp"

#include "util/common.h"

namespace bolt {
namespace gfx {

SceneManager::SceneManager() : mSceneRoot(math::Matrix44f::IDENTITY) {
    mRenderSystem = new OpenglRenderSystem;
}

SceneManager::~SceneManager() {
    for (Camera* camera : mCameras) {
        delete camera;
    }
    for (Drawable3d* drawable : mDrawables) {
        delete drawable;
    }
    delete mRenderSystem;
}

OrbitCamera* SceneManager::createOrbitCamera() {
    auto* cam = new OrbitCamera;
    mCameras.push_back(cam);
    return cam;
}

void SceneManager::loadAll() {
    addDrawableRecurse(&mSceneRoot);

    mRenderSystem->load();
}

void SceneManager::addDrawableRecurse(SceneNode* node) {
    RUNTIME_ASSERT(node != nullptr, "null node");
    if (node->type() == SceneNode::NODE_TYPE_DRAWABLE) {
        mRenderSystem->addDrawable(static_cast<Drawable3d*>(node));
    }
    for (int i = 0; i < node->childCount(); i++) {
        addDrawableRecurse(node->child(i));
    }
}

void SceneManager::draw(Camera* camera) {
    mSceneRoot.updateWorldTransforms();
    mRenderSystem->renderFrame(*camera);
}

void SceneManager::draw() {
    for (Camera* camera : mCameras) {
        draw(camera);
    }
}

} // namespace gfx
} // namespace bolt
