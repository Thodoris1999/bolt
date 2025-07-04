#include "Object.hpp"

using namespace bolt::math;

Object3d::Object3d() : mSceneNode(nullptr), mCollider(nullptr) {}

Object3d::~Object3d() {
    delete mCollider;
}

void Object3d::setPose(Vector3f pos, Vector3f rot) {
    mMtx.setTranslation(pos.x, pos.y, pos.z);
    mMtx.setRotation(rot.x, rot.y, rot.z);
    updateMatrix();
}

void Object3d::updateMatrix() {
    if (mSceneNode != nullptr)
        mSceneNode->setMtx(mMtx);
    if (mCollider != nullptr)
        mCollider->setMtx(mMtx);
}
