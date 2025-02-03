#pragma once

#include "Drawable.hpp"
#include "Camera.hpp"

#include <vector>

/**
 * Simple linear container for graphics objects
 */
class Drawable3dManager {
public:
    Drawable3dManager();
    ~Drawable3dManager();
    void init();
    void draw();
    void registerDrawable(Drawable3d* drawable);

    Camera* camera() { return mCamera; }

private:
    std::vector<Drawable3d*> mDrawables;
    Camera* mCamera;
};
