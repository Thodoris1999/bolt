#include "ShapeSphere.hpp"

ShapeSphere::ShapeSphere(float radius) : mCenter(0, 0, 0), mRadius(radius) {

}

void ShapeSphere::setMtx(const Matrix34f& mat) {
    mCenter.x = mat(0, 3);
    mCenter.y = mat(1, 3);
    mCenter.z = mat(2, 3);
}

void ShapeSphere::getSupport(const Vector3f& dir, Vector3f& support) {
    support = mCenter + mRadius * dir;
}
