#include "ObjectModel.hpp"

#include "gfx/DrawableModel.hpp"

ObjectModel::ObjectModel(const char* path) {
    mDrawable = new DrawableModel(path);
}
