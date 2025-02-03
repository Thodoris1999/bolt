#pragma once

#include "math/Matrix.hpp"

/** \defgroup ColliderFlags Collider Flags
 *  Possible bitfield slots for \ref Collider flags
 */
///@{
/// Notes that the object is convex and can be cast to \ref ColliderConvex
#define COLLIDER_FLAG_CONVEX 1
///@}

/**
 * Base class representing a shape that has collision
 */
class Collider {
public:
    Collider(int flags) : mFlags(flags) {}
    virtual ~Collider() {}

    virtual void setMtx(const Matrix34f&) = 0;

    const int flags() const { return mFlags; }
    int& flags() { return mFlags; }
    bool isConvex() const { return (mFlags & COLLIDER_FLAG_CONVEX) != 0; }

protected:
    /// Bitfield for flags that hint the collision algorithm dispatcher on what can be assumed
    /// about this collider, see \ref ColliderFlags
    int mFlags;
};
