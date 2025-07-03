#include "gfx/Camera.hpp"
#include "gfx/gl_defines.h"

#include "math/math.h"

namespace bolt {
namespace gfx {

using namespace math;

Camera::Camera() {
    mMtx.makeHomogeneous();
    // note that we're translating the scene in the reverse direction of where we want to move
    mMtx.setRotation(0, 0, 0);
    mMtx.setTranslation(0, 0, -3);

    mProjection.setPerspective(DEG2RAD(45), 1, 0.1, 100);

    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(Matrix44f), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glCheckError();
}

void Camera::onDraw() {
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 2 * sizeof(Matrix44f));
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix44f), &mProjection);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix44f), sizeof(Matrix44f), &mMtx);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

OrbitCamera::OrbitCamera() : mFocus(0, 0, 0), mPos(10, 10, 10), mFovy(DEG2RAD(45)), mAspectRatio(1) {
    updatePerspectiveMat();
    mMtx.setLookAt(mPos, mFocus, Vector3f(0, 0, 1));
}

void OrbitCamera::setPos(const math::Vector3f& pos) {
    mPos = pos;
    mMtx.setLookAt(mPos, mFocus, Vector3f(0, 0, 1));
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
}

void OrbitCamera::setFocus(const math::Vector3f& focus) {
    mFocus = focus;
    mMtx.setLookAt(mPos, mFocus, Vector3f(0, 0, 1));
}

void OrbitCamera::onScroll(float amount) {
    mFovy += -amount;
    mFovy = mFovy < 0 ? 0 : mFovy;
    mFovy = mFovy > 120 ? 120 : mFovy;

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

    // Update the view matrix
    mMtx.setLookAt(mPos, mFocus, Vector3f(0, 0, 1));
}

} // gfx
} // bolt
