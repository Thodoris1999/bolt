#include "gfx/Camera.hpp"

#include "math/math.h"

#include "util/common.h"

namespace bolt {
namespace gfx {

using namespace math;

Camera::Camera() : SceneNode(SceneNode::NODE_TYPE_CAMERA) {
    mMtx.makeHomogeneous();
    // note that we're translating the scene in the reverse direction of where we want to move
    mMtx.setRotation(0, 0, 0);
    mMtx.setTranslation(0, 0, -3);

    mProjection.setPerspective(DEG2RAD(45), 1, 0.1, 100);
}

Matrix44f Camera::getView() const {
#ifndef NDEBUG
    RUNTIME_ASSERT(!mWorldDirty, "View matrix is only valid if the world matrix is valid");
#endif
    return mWorldMtx.rigidInverse();
}

OrbitCamera::OrbitCamera() : mFocus(0, 0, 0), mPos(10, 10, 10), mFovy(DEG2RAD(45)), mAspectRatio(1) {
    updatePerspectiveMat();
    mMtx.setLookAt(mPos, mFocus, Vector3f(0, 0, 1));
}

void OrbitCamera::setPos(const math::Vector3f& pos) {
    mPos = pos;
    mMtx.setLookAt(mPos, mFocus, Vector3f(0, 0, 1));
    mWorldDirty = true;
}

void OrbitCamera::setRot(const math::Quatf& rot) {
    float qw = rot.w;
    float qx = rot.x;
    float qy = rot.y;
    float qz = rot.z;

    mMtx(0,0) = 1.0f - 2.0f*qy*qy - 2.0f*qz*qz;
    mMtx(0,1) = 2.0f*qx*qy - 2.0f*qz*qw;
    mMtx(0,2) = 2.0f*qx*qz + 2.0f*qy*qw;

    mMtx(1,0) = 2.0f*qx*qy + 2.0f*qz*qw;
    mMtx(1,1) = 1.0f - 2.0f*qx*qx - 2.0f*qz*qz;
    mMtx(1,2) = 2.0f*qy*qz - 2.0f*qx*qw;

    mMtx(2,0) = 2.0f*qx*qz - 2.0f*qy*qw;
    mMtx(2,1) = 2.0f*qy*qz + 2.0f*qx*qw;
    mMtx(2,2) = 1.0f - 2.0f*qx*qx - 2.0f*qy*qy;
    mWorldDirty = true;
}

void OrbitCamera::setFocus(const math::Vector3f& focus) {
    mFocus = focus;
    mMtx.setLookAt(mPos, mFocus, Vector3f(0, 0, 1));
    mWorldDirty = true;
}

void OrbitCamera::onScroll(float amount) {
    mFovy += -amount;
    mFovy = mFovy < 0 ? 0 : mFovy;
    float maxFov = 4 * M_PI / 5;
    mFovy = mFovy > maxFov ? maxFov : mFovy;

    updatePerspectiveMat();
}

void OrbitCamera::onDrag(float x, float y) {
    // Calculate spherical coordinates for the position relative to the focus
    Vector3f direction = mPos - mFocus;
    float radius = direction.length();
    float theta = atan2(direction.y, direction.x); // azimuthal angle
    float phi = acos(direction.z / radius);       // polar angle

    // Update angles based on drag input
    theta -= x * 0.01f; // Adjust sensitivity as needed
    phi -= y * 0.01f;   // Adjust sensitivity as needed

    // Clamp phi to avoid flipping (polar angle within (0, PI))
    const float epsilon = 0.01f; // Small offset to prevent singularity at poles
    phi = std::max(epsilon, std::min(static_cast<float>(M_PI) - epsilon, phi));

    // Convert spherical coordinates back to Cartesian coordinates
    direction.x = radius * sin(phi) * cos(theta);
    direction.y = radius * sin(phi) * sin(theta);
    direction.z = radius * cos(phi);

    // Update the camera position
    mPos = mFocus + direction;

    // Update the transformation matrix
    mMtx.setLookAt(mPos, mFocus, Vector3f(0, 0, 1));
    mWorldDirty = true;
}

} // gfx
} // bolt
