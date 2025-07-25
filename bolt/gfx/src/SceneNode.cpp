#include "gfx/SceneNode.hpp"

namespace bolt {
namespace gfx {

SceneNode::SceneNode(Type type) : mType(type) {
    mParent = nullptr;
    mWorldDirty = true;
    mMtx.setIdentity();
}

SceneNode::SceneNode(math::Matrix44f mtx, Type type) : mMtx(mtx), mType(type) {
    mParent = nullptr;
    mWorldDirty = true;
}

void SceneNode::addChild(SceneNode* node) {
    node->mParent = this;
    mChildren.push_back(node);
}

void SceneNode::setPose(const math::Vector3f& pos, const math::Vector3f& rot) {
    mMtx.setTranslation(pos.x, pos.y, pos.z);
    mMtx.setRotation(rot.x, rot.y, rot.z);
    mWorldDirty = true;
}

void SceneNode::setRotation(float x, float y, float z) {
    mMtx.setRotation(x, y, z);
    mWorldDirty = true;
}

void SceneNode::setTranslation(float x, float y, float z) {
    mMtx.setTranslation(x, y, z);
    mWorldDirty = true;
}

void SceneNode::setMtx(const bolt::math::Matrix34f& mtx) {
    mMtx(0,0) = mtx(0,0);
    mMtx(0,1) = mtx(0,1);
    mMtx(0,2) = mtx(0,2);
    mMtx(0,3) = mtx(0,3);
    mMtx(1,0) = mtx(1,0);
    mMtx(1,1) = mtx(1,1);
    mMtx(1,2) = mtx(1,2);
    mMtx(1,3) = mtx(1,3);
    mMtx(2,0) = mtx(2,0);
    mMtx(2,1) = mtx(2,1);
    mMtx(2,2) = mtx(2,2);
    mMtx(2,3) = mtx(2,3);
    mWorldDirty = true;
}

void SceneNode::updateWorldTransforms() {
    if (mWorldDirty) {
        if (mParent != nullptr) {
            mWorldMtx = mParent->worldMtx() * mMtx;
        } else {
            mWorldMtx = mMtx;
        }
        
        mWorldDirty = false;
    }

    for (SceneNode* child : mChildren) {
        child->updateWorldTransforms();
    }
}

} // gfx
} // bolt
