#pragma once

#include "ColliderConvexHull.hpp"

#include "math/Vector.hpp"

#include <vector>

class ShapeCuboid : public ColliderConvexHull {
public:
    ShapeCuboid(float sizeX, float sizeY, float sizeZ);
    ~ShapeCuboid() {}
};
