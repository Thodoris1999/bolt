#pragma once

#include "ColliderConvex.hpp"

#include "math/Vector.hpp"

#include <vector>

namespace bolt {
namespace col {

class ColliderConvexHull : public ColliderConvex {
public:
    ColliderConvexHull(std::vector<math::Vector3f> points);
    ~ColliderConvexHull() {}

    virtual void getSupport(const math::Vector3f&, math::Vector3f&) override;
    virtual void getCenter(math::Vector3f&) override;
    virtual void setMtx(const math::Matrix34f& mat) override;

protected:
    std::vector<math::Vector3f> mPoints;
    std::vector<math::Vector3f> mWorldPoints;
};

} // col
} // bolt
