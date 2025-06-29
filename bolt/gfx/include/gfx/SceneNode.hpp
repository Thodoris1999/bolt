#pragma once

#include "math/Matrix.hpp"

#include <vector>

namespace bolt {
namespace gfx {

class SceneNode {
public:
    // dtors/ctors
    SceneNode();
    virtual ~SceneNode() {}

    /// @beginGetters
    math::Matrix44f& mtx() { return mMtx; }
    const math::Matrix44f& worldMtx() const { return mWorldMtx; }
    /// @endGetters

    /// @beginSetters
    void setMtx(const math::Matrix34f& mtx);
    /// @endSetters

    // graph editing
    void addChild(SceneNode* node);

    // graph transactions
    void setTransform(const bolt::math::Matrix44f& tf);
    void updateWorldTransforms();

protected:
    SceneNode* mParent;
    std::vector<SceneNode*> mChildren;
    math::Matrix44f mMtx;
    math::Matrix44f mWorldMtx;
    bool mWorldDirty;
};

} // gfx
} // bolt
