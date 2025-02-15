#pragma once

#include "math/Vector.hpp"
#include "math/Matrix.hpp"

#include "col/Collider.hpp"

namespace bolt {
namespace col {

class ColliderConvex : public Collider {
public:
    ColliderConvex() : Collider(COLLIDER_FLAG_CONVEX) {}
    virtual ~ColliderConvex() {}

    virtual void getSupport(const math::Vector3f&, math::Vector3f&) = 0;
};

bool checkConvexCollision(const ColliderConvex& c1, const ColliderConvex& c2);

} // col
} // bolt