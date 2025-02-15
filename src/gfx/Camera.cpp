#include "Camera.hpp"

#include "math/math.h"

#include "gl_defines.h"

namespace bolt {
namespace gfx {

using namespace math;

Camera::Camera() {
    mView.makeHomogeneous();
    // note that we're translating the scene in the reverse direction of where we want to move
    mView.setRotation(0, 0, 0);
    mView.setTranslation(0, 0, -3);

    mProjection.setPerspective(DEG2RAD(45), 1, 0.1, 100);

    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(Matrix44f), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 2 * sizeof(Matrix44f));
    glCheckError();
}

void Camera::onDraw() {
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix44f), &mProjection);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix44f), sizeof(Matrix44f), &mView);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

OrbitCamera::OrbitCamera() : mFocus(0, 0, 0), mPos(10, 10, 10), mFovy(45) {
    mProjection.setPerspective(DEG2RAD(mFovy), 1, 0.1, 100);
    mView.setLookAt(mPos, mFocus, Vector3f(0, 0, 1));
}

void OrbitCamera::onScroll(float amount) {
    mFovy += -amount;
    mFovy = mFovy < 0 ? 0 : mFovy;
    mFovy = mFovy > 120 ? 120 : mFovy;

    mProjection.setPerspective(DEG2RAD(mFovy), 1, 0.1, 100);
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
    mView.setLookAt(mPos, mFocus, Vector3f(0, 0, 1));
}

} // gfx
} // bolt