#pragma once

#include "Object.hpp"

class ObjectCuboid : public Object3d {
public:
    ObjectCuboid(float sizeX, float sizeY, float sizeZ);
    virtual ~ObjectCuboid() {}
};
