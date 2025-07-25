#pragma once

#include "Object.hpp"

#include "gfx/DrawableModel.hpp"

class ObjectModel : public Object3d {
public:
    ObjectModel(const char* path);
    virtual ~ObjectModel();
    virtual void createVisuals(bolt::gfx::SceneManager& scene) override;

private:
    bolt::gfx::DrawableModel mModel;
};
