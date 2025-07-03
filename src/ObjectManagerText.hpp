#pragma once

#include <string>
#include <vector>

#include <gfx/SceneManager.hpp>
#include <col/ColliderManager.hpp>

#include "obj/Object.hpp"

class ObjectManagerText {
public:
    ObjectManagerText(bolt::gfx::SceneManager* scene) : mScene(scene) {}
    ~ObjectManagerText();

    void createObjects(const std::string&);
    void createObjects(const char* path);

    // Register object shapes to a collider manager
    void registerColliders(bolt::col::ColliderManager&);

private:
    Object3d* createObject(const std::string& text);

    std::vector<Object3d*> mObjects;
    bolt::gfx::SceneManager* mScene;
};
