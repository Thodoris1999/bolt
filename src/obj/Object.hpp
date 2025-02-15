#pragma once

#include "gfx/Drawable.hpp"
#include "col/Collider.hpp"

/**
 * An Object is a convenience entity that manages a Drawable and a Collider. Simple as that
 */
class Object3d {
public:
    Object3d();
    virtual ~Object3d();

    bolt::gfx::Drawable3d* drawable() { return mDrawable; }
    bolt::col::Collider* collider() { return mCollider; }

    void setPose(bolt::math::Vector3f pos, bolt::math::Vector3f rot);

protected:
    bolt::gfx::Drawable3d* mDrawable;
    bolt::col::Collider* mCollider;
    bolt::math::Matrix34f mMtx;

private:
    void updateMatrix();
};
