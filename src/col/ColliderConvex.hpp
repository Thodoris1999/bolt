#pragma once

#include "math/Vector.hpp"
#include "math/Matrix.hpp"

#include "col/Collider.hpp"

class ColliderConvex : public Collider {
public:
    ColliderConvex() : Collider(COLLIDER_FLAG_CONVEX) {}
    virtual ~ColliderConvex() {}

    virtual void getSupport(const Vector3f&, Vector3f&) = 0;
};

bool checkConvexCollision(const ColliderConvex& c1, const ColliderConvex& c2);
