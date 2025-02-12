#include "ObjectMesh.hpp"

#include "gfx/DrawableModel.hpp"

ObjectMesh::ObjectMesh(const char* path) {
    mDrawable = new DrawableModel(path);
}
