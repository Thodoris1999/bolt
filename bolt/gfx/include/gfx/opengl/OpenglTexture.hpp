#pragma once

#include "gfx/RenderTexture.hpp"

#include "gfx/opengl/gl_defines.h"

namespace bolt {
namespace gfx {

class OpenglTexture final : public RenderTexture {
public:
    OpenglTexture(const char* textureFile, GLint samplerLocation);

    GLint samplerLocation() const { return mSamplerLocation; }

    virtual void bind() override;

private:
    GLuint mId;
    GLint mSamplerLocation;
};

} // gfx
} // bolt
