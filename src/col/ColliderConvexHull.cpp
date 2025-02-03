#include "ColliderConvexHull.hpp"

#include <float.h>

ColliderConvexHull::ColliderConvexHull(std::vector<Vector3f> points) : mPoints(points) {
    mWorldPoints.resize(points.size());
}

void ColliderConvexHull::setMtx(const Matrix34f& mat) {
    for (int i = 0; i < mPoints.size(); i++) {
        mWorldPoints[i] = mat * mPoints[i];
    }
}

void ColliderConvexHull::getSupport(const Vector3f& dir, Vector3f& support) {
    float max = -FLT_MAX;
    for (const auto& worldPoint : mWorldPoints) {
        float dot = worldPoint.dot(dir);
        if (dot > max) {
            max = dot;
            support = worldPoint;
        }
    }
}
