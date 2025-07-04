#include "ObjectModel.hpp"

#include "gfx/DrawableModel.hpp"

#include <string.h>

using namespace bolt;

ObjectModel::ObjectModel(const char* path) {
    mPath = new char[strlen(path) + 1]; // +1 for null terminator
    strcpy(mPath, path);
}

ObjectModel::~ObjectModel() {
    delete mPath;
}

void ObjectModel::createVisuals(gfx::SceneManager& scene) {
    if (mSceneNode == nullptr) {
        auto* model = scene.createDrawable<gfx::DrawableModel>(mPath);
        model->setMtx(mMtx);
        mSceneNode = model;
    }
}
