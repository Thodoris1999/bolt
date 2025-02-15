#pragma once

#include "ColliderConvex.hpp"

namespace bolt {
namespace col {

class ShapeSphere : public ColliderConvex {
public:
    ShapeSphere(float radius);
    ~ShapeSphere() {}

    virtual void getSupport(const math::Vector3f&, math::Vector3f&) override;
    virtual void setMtx(const math::Matrix34f& mat) override;

private:
    math::Vector3f mCenter;
    float mRadius;
};

} // col
} // bolt