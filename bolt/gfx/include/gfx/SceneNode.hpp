#pragma once

#include "math/Matrix.hpp"
#include "math/Vector.hpp"

#include <vector>

namespace bolt {
namespace gfx {

class SceneNode {
public:
    /// lightweight custom RTTI for scene nodes
    enum Type {
        /// just a regular node providing just a coordinate frame
        NODE_TYPE_PLAIN,
        /// node also inheriting Drawable
        NODE_TYPE_DRAWABLE,
        /// node also inheriting Camera
        NODE_TYPE_CAMERA,
    };

    // dtors/ctors
    SceneNode(Type type = NODE_TYPE_PLAIN);
    SceneNode(math::Matrix44f mtx, Type type = NODE_TYPE_PLAIN);
    virtual ~SceneNode() {}

    /// @beginGetters
    const math::Matrix44f& mtx() const { return mMtx; }
    const math::Matrix44f& worldMtx() const { return mWorldMtx; }
    SceneNode* child(int i) { return mChildren[i]; }
    int childCount() const { return mChildren.size(); }
    Type type() const { return mType; }
    /// @endGetters

    /// @beginSetters
    void setMtx(const math::Matrix34f& mtx);
    void setPose(const math::Vector3f& pos, const math::Vector3f& rot);
    void setRotation(float roll, float pitch, float yaw);
    void setTranslation(float x, float y, float z);
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
    Type mType;
};

} // gfx
} // bolt
