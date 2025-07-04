#pragma once

#include "Object.hpp"

#include "gfx/SceneManager.hpp"

class ObjectSphere : public Object3d {
public:
    ObjectSphere(float radius);
    virtual void createVisuals(bolt::gfx::SceneManager& scene) override;

private:
    float mRadius;
};
