#pragma once

#include "gfx/Camera.hpp"
#include "gfx/Drawable.hpp"

namespace bolt {
namespace gfx {

class RenderSystem {
public:
    virtual void setViewport(int x, int y, int width, int height) = 0;
    virtual void addDrawable(Drawable* drawable) = 0;
    virtual void load() = 0;
    virtual void renderFrame(const Camera& camera) = 0;
};

} // gfx
} // bolt
