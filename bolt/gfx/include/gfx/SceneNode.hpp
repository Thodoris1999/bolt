#pragma once

#include "math/Matrix.hpp"
#include "math/Vector.hpp"
#include "math/Quat.hpp"

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
    math::Vector3f worldPos() const { return mWorldMtx.getTranslation(); }
    SceneNode* child(int i) { return mChildren[i]; }
    const SceneNode* child(int i) const { return mChildren[i]; }
    int childCount() const { return mChildren.size(); }
    SceneNode* parent() { return mParent; }
    const SceneNode* parent() const { return mParent; }
    Type type() const { return mType; }
    /// @endGetters

    /// @beginSetters
    void setMtx(const math::Matrix34f& mtx);
    void setMtx(const math::Matrix44f& mtx) { mMtx = mtx; mWorldDirty = true; }
    void setPose(const math::Vector3f& pos, const math::Vector3f& rot);
    void setRotation(const math::Quatf& q);
    void setRotation(float roll, float pitch, float yaw);
    void setTranslation(const math::Vector3f& pos);
    void setTranslation(float x, float y, float z);
    /// @endSetters

    // graph editing
    void addChild(SceneNode* node);
    /// Detaches node from this node's children. Does not destroy it
    void detachChild(SceneNode* node);

    // graph transactions
    virtual void updateWorldTransforms(bool parentDirty = false);

private:
    void setWorldDirty() { mWorldDirty = true; }

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
