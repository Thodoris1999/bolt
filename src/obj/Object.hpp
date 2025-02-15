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
    Collider* collider() { return mCollider; }

    void setPose(Vector3f pos, Vector3f rot);

protected:
    bolt::gfx::Drawable3d* mDrawable;
    Collider* mCollider;
    Matrix34f mMtx;

private:
    void updateMatrix();
};
