#pragma once

#include "ColliderConvex.hpp"

class ShapeSphere : public ColliderConvex {
public:
    ShapeSphere(float radius);
    ~ShapeSphere() {}

    virtual void getSupport(const Vector3f&, Vector3f&) override;
    virtual void setMtx(const Matrix34f& mat) override;

private:
    Vector3f mCenter;
    float mRadius;
};
