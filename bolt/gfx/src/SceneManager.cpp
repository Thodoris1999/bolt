#include "gfx/SceneManager.hpp"
#include "gfx/opengl/OpenglRenderSystem.hpp"

#include "util/common.h"

namespace bolt {
namespace gfx {

struct CameraData {
    math::Matrix44f projection;
    math::Matrix44f view;
};

SceneManager::SceneManager() : mSceneRoot(math::Matrix44f::IDENTITY) {
    // more render systems can be supported in the future by extending RenderSystem
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

    // create uniform blocks
    // binding point 0: camera view and projection matrices
    mUniforms.emplace_back(mRenderSystem->addUniform(sizeof(CameraData), 0));
    // binding point 1: camera position
    mUniforms.emplace_back(mRenderSystem->addUniform(sizeof(math::Vector3f), 1));
    // binding point 2: directional light
    mUniforms.emplace_back(mRenderSystem->addUniform(sizeof(DirLightParams), 2));

    // add a sun
    mDirLightParams = DirLightParams(
        math::Vector3f(-0.2f, -0.3f, -1.0f),   // direction
        math::Vector3f(0.3f, 0.3f, 0.3f),   // ambient
        math::Vector3f(0.4f, 0.4f, 0.4f),      // diffuse
        math::Vector3f(0.5f, 0.5f, 0.5f)       // specular
    );
    mRenderSystem->uniform(2)->writeData(&mDirLightParams, 0, sizeof(mDirLightParams));
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
    // update transforms
    mSceneRoot.updateWorldTransforms();

    // update uniforms
    CameraData cameraData;
    cameraData.projection = camera->getProjection();
    cameraData.view = camera->getView();
    mRenderSystem->uniform(0)->writeData(&cameraData, 0, sizeof(cameraData));
    math::Vector3f camPos = camera->worldPos();
    mRenderSystem->uniform(1)->writeData(&camPos, 0, sizeof(math::Vector3f));

    // draw frame
    mRenderSystem->renderFrame();
}

void SceneManager::draw() {
    for (Camera* camera : mCameras) {
        draw(camera);
    }
}

} // namespace gfx
} // namespace bolt
