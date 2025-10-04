#include "ObjectSphere.hpp"

#include "gfx/DrawableSpheroid.hpp"
#include "col/ShapeSphere.hpp"

using namespace bolt;

ObjectSphere::ObjectSphere(float radius) : mRadius(radius) {
    mCollider = new bolt::col::ShapeSphere(radius);
}

void ObjectSphere::createVisuals(gfx::SceneManager& scene) {
    if (mSceneNode == nullptr) {
        auto* sphere = scene.createDrawable<gfx::DrawableSpheroid>(mRadius, mRadius, mRadius);
        sphere->setMaterial(gfx::randomBrightMaterial());
        sphere->setMtx(mMtx);
        mSceneNode = sphere;
    }
}
