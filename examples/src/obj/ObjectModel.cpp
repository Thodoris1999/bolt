#include "ObjectModel.hpp"

#include <string.h>

using namespace bolt;

ObjectModel::ObjectModel(const char* path) : mModel(path) {
}

ObjectModel::~ObjectModel() {
}

void ObjectModel::createVisuals(gfx::SceneManager& scene) {
    if (mSceneNode == nullptr) {
        mModel.setMtx(mMtx);
        mModel.load();
        mSceneNode = &mModel;
    }
}
