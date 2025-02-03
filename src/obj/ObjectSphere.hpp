#pragma once

#include "Object.hpp"

class ObjectSphere : public Object3d {
public:
    ObjectSphere(float radius);
    virtual ~ObjectSphere() {}
};
