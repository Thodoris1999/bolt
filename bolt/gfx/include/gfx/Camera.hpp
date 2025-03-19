#pragma once

#include "math/Matrix.hpp"
#include "math/Vector.hpp"

namespace bolt {
namespace gfx {

class Camera {
public:
    Camera();
    virtual ~Camera() {}
    /// send view and projection matrices to GPU
    void onDraw();

    // controls
    virtual void onScroll(float amount) {}
    virtual void onDrag(float x, float y) {}

    math::Matrix44f& getView() { return mView; }
    math::Matrix44f& getProjection() { return mProjection; }

protected:
    unsigned int UBO;
    math::Matrix44f mView;
    math::Matrix44f mProjection;
};

class OrbitCamera : public Camera {
public:
    OrbitCamera();
    virtual void onScroll(float amount) override;
    virtual void onDrag(float x, float y) override;

private:
    math::Vector3f mPos;
    math::Vector3f mFocus;
    float mFovy;
};

} // gfx
} // bolt