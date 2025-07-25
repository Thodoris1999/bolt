#pragma once

namespace bolt {
namespace gfx {

/**
 * Interface over rendering API textures
 */
class RenderTexture {
public:
    virtual ~RenderTexture() {}
    // bind texture
    virtual void bind() = 0;
};

} // gfx
} // bolt

