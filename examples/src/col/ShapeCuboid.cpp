#include "ShapeCuboid.hpp"

#include "math/Vector.hpp"

#include <vector>

namespace bolt {
namespace col {

static std::vector<math::Vector3f> createPoints(float sizeX, float sizeY, float sizeZ) {
    float halfX = sizeX / 2.0f;
    float halfY = sizeY / 2.0f;
    float halfZ = sizeZ / 2.0f;

    std::vector<math::Vector3f> ret = {
        { -halfX, -halfY, -halfZ },
        {  halfX, -halfY, -halfZ },
        {  halfX,  halfY, -halfZ },
        { -halfX,  halfY, -halfZ },
        { -halfX, -halfY,  halfZ },
        {  halfX, -halfY,  halfZ },
        {  halfX,  halfY,  halfZ },
        { -halfX,  halfY,  halfZ } 
    };

    return ret;
}

ShapeCuboid::ShapeCuboid(float sizeX, float sizeY, float sizeZ) : ColliderConvexHull(createPoints(sizeX, sizeY, sizeZ)) {
}

} // col
} // bolt