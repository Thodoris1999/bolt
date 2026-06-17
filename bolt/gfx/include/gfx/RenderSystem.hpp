#pragma once

#include "gfx/Drawable.hpp"
#include "gfx/RenderUniformBuffer.hpp"

namespace bolt {
namespace gfx {

class RenderSystem {
public:
    virtual ~RenderSystem() {}
    virtual void setClearColor(float r, float g, float b, float a) = 0;
    virtual void setViewport(int x, int y, int width, int height) = 0;
    virtual void addDrawable(Drawable* drawable) = 0;
    virtual RenderUniformBuffer* addUniform(size_t size, uint32_t bindPoint) = 0;
    /**
     * Loads all necessary things for RenderSystem to be able to render frames. Call only once, after adding all desired drawables and uniforms
     */
    virtual void load() = 0;
    virtual void renderFrame() = 0;

    /// Correction matrix applied to the projection matrix to account for backend-specific clip-space
    /// conventions (NDC Y direction, depth range). Identity by default (OpenGL conventions).
    virtual math::Matrix44f clipSpaceCorrection() const { return math::Matrix44f::IDENTITY; }

    RenderUniformBuffer* uniform(int i) { return mUniforms[i]; }

protected:
    std::vector<RenderUniformBuffer*> mUniforms;
};

} // gfx
} // bolt
