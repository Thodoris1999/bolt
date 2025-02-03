#include "ObjectCuboid.hpp"

#include "gfx/DrawableCuboid.hpp"
#include "col/ShapeCuboid.hpp"

ObjectCuboid::ObjectCuboid(float sizeX, float sizeY, float sizeZ) {
    mDrawable = new DrawableCuboid(sizeX, sizeY, sizeZ);
    mCollider = new ShapeCuboid(sizeX, sizeY, sizeZ);
}
