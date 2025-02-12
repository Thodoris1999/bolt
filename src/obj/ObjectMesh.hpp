#pragma once

#include "Object.hpp"

class ObjectMesh : public Object3d {
public:
    ObjectMesh(const char* path);
    virtual ~ObjectMesh() {}
};
