#include "ObjectSphere.hpp"

#include "gfx/DrawableSpheroid.hpp"
#include "col/ShapeSphere.hpp"

ObjectSphere::ObjectSphere(float radius) {
    mDrawable = new DrawableSpheroid(radius, radius, radius);
    mCollider = new ShapeSphere(radius);
}
