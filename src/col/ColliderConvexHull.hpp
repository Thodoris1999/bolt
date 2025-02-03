#pragma once

#include "ColliderConvex.hpp"

#include "math/Vector.hpp"

#include <vector>

class ColliderConvexHull : public ColliderConvex {
public:
    ColliderConvexHull(std::vector<Vector3f> points);
    ~ColliderConvexHull() {}

    virtual void getSupport(const Vector3f&, Vector3f&) override;
    virtual void setMtx(const Matrix34f& mat) override;

protected:
    std::vector<Vector3f> mPoints;
    std::vector<Vector3f> mWorldPoints;
};
