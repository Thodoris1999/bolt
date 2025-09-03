#include "ObjectCuboid.hpp"

#include "gfx/DrawableCuboid.hpp"
#include "col/ShapeCuboid.hpp"

using namespace bolt;

ObjectCuboid::ObjectCuboid(float sizeX, float sizeY, float sizeZ) : mSx(sizeX), mSy(sizeY), mSz(sizeZ) {
    mCollider = new bolt::col::ShapeCuboid(sizeX, sizeY, sizeZ);
}

void ObjectCuboid::createVisuals(gfx::SceneManager& scene) {
    if (mSceneNode == nullptr) {
        auto* cuboid = scene.createDrawable<gfx::DrawableCuboid>(mSx, mSy, mSz);
        cuboid->setMaterial(gfx::randomBrightMaterial());
        cuboid->setMtx(mMtx);
        mSceneNode = cuboid;
    }
}
