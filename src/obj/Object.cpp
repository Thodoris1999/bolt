#include "Object.hpp"

Object3d::Object3d() : mDrawable(nullptr), mCollider(nullptr) {}

Object3d::~Object3d() {
    delete mDrawable;
    delete mCollider;
}

void Object3d::setPose(Vector3f pos, Vector3f rot) {
    mMtx.setTranslation(pos.x, pos.y, pos.z);
    mMtx.setRotation(rot.x, rot.y, rot.z);
    updateMatrix();
}

void Object3d::updateMatrix() {
    if (mDrawable != nullptr)
        mDrawable->setMtx(mMtx);
    if (mCollider != nullptr)
        mCollider->setMtx(mMtx);
}
