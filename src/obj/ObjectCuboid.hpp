#pragma once

#include "Object.hpp"

#include "gfx/SceneManager.hpp"

class ObjectCuboid : public Object3d {
public:
    ObjectCuboid(float sizeX, float sizeY, float sizeZ);
    virtual void createVisuals(bolt::gfx::SceneManager& scene) override;

private:
    float mSx;
    float mSy;
    float mSz;
};
