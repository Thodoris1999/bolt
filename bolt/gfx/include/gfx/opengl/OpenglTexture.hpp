#pragma once

#include "gfx/RenderTexture.hpp"

#include "gfx/opengl/gl_defines.h"

namespace bolt {
namespace gfx {

class OpenglTexture final : public RenderTexture {
public:
    OpenglTexture(const char* textureFile, uint32_t binding);
    virtual ~OpenglTexture();

    GLuint id() const { return mId; }
    GLuint binding() const { return mBinding; }

    void bind();

private:
    GLuint mId;
    GLint mBinding;
};

} // gfx
} // bolt
