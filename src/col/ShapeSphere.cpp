#include "ShapeSphere.hpp"

namespace bolt {
namespace col {

ShapeSphere::ShapeSphere(float radius) : mCenter(0, 0, 0), mRadius(radius) {

}

void ShapeSphere::setMtx(const math::Matrix34f& mat) {
    mCenter.x = mat(0, 3);
    mCenter.y = mat(1, 3);
    mCenter.z = mat(2, 3);
}

void ShapeSphere::getSupport(const math::Vector3f& dir, math::Vector3f& support) {
    math::Vector3f unitDir = dir.normalize();
    support = mCenter + mRadius * unitDir;
}

} // col
} // bolt