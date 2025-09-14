#pragma once

#include "gfx/Drawable.hpp"
#include "gfx/RenderUniformBuffer.hpp"

namespace bolt {
namespace gfx {

class RenderSystem {
public:
    virtual void setViewport(int x, int y, int width, int height) = 0;
    virtual void addDrawable(Drawable* drawable) = 0;
    virtual RenderUniformBuffer* addUniform(size_t size, uint32_t bindPoint) = 0;
    virtual void load() = 0;
    virtual void renderFrame() = 0;

    RenderUniformBuffer* uniform(int i) { return mUniforms[i]; }

protected:
    std::vector<RenderUniformBuffer*> mUniforms;
};

} // gfx
} // bolt
