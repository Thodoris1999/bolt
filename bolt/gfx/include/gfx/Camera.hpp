#pragma once

#include "math/Matrix.hpp"
#include "math/Vector.hpp"
#include "math/Quat.hpp"

#include "gfx/SceneNode.hpp"

namespace bolt {
namespace gfx {

class Camera : public SceneNode {
public:
    Camera();
    virtual ~Camera() {}

    // controls
    virtual void onScroll(float amount) {}
    virtual void onDrag(float x, float y) {}
    virtual void setAspectRatio(float aspectRatio) {}

    math::Matrix44f& getProjection() { return mProjection; }
    const math::Matrix44f& getProjection() const { return mProjection; }

protected:
    math::Matrix44f mProjection;
};

class OrbitCamera : public Camera {
public:
    OrbitCamera();
    void setFocus(const math::Vector3f& focus);
    virtual void onScroll(float amount) override;
    virtual void onDrag(float x, float y) override;

    void setAspectRatio(float aspectRatio) override { mAspectRatio = aspectRatio; updatePerspectiveMat(); }

    void setPos(const math::Vector3f& rot);
    void setRot(const math::Quatf& rot);

private:
    void updatePerspectiveMat() { mProjection.setPerspective(mFovy, mAspectRatio, 0.1, 100); }

    math::Vector3f mPos;
    math::Vector3f mFocus;
    float mFovy;
    float mAspectRatio;
};

} // gfx
} // bolt
