#pragma once

#include "math/Matrix.hpp"
#include "math/Vector.hpp"

class Camera {
public:
    Camera();
    virtual ~Camera() {}
    /// send view and projection matrices to GPU
    void onDraw();

    // controls
    virtual void onScroll(float amount) {}
    virtual void onDrag(float x, float y) {}

    Matrix44f& getView() { return mView; }
    Matrix44f& getProjection() { return mProjection; }

protected:
    unsigned int UBO;
    Matrix44f mView;
    Matrix44f mProjection;
};

class OrbitCamera : public Camera {
public:
    OrbitCamera();
    virtual void onScroll(float amount) override;
    virtual void onDrag(float x, float y) override;

private:
    Vector3f mPos;
    Vector3f mFocus;
    float mFovy;
};
