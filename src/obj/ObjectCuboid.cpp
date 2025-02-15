#include "ObjectCuboid.hpp"

#include "gfx/DrawableCuboid.hpp"
#include "col/ShapeCuboid.hpp"

ObjectCuboid::ObjectCuboid(float sizeX, float sizeY, float sizeZ) {
    mDrawable = new bolt::gfx::DrawableCuboid(sizeX, sizeY, sizeZ);
    mCollider = new bolt::col::ShapeCuboid(sizeX, sizeY, sizeZ);
}
