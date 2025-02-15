#include "ObjectSphere.hpp"

#include "gfx/DrawableSpheroid.hpp"
#include "col/ShapeSphere.hpp"

ObjectSphere::ObjectSphere(float radius) {
    mDrawable = new bolt::gfx::DrawableSpheroid(radius, radius, radius);
    mCollider = new ShapeSphere(radius);
}
