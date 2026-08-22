#pragma once

#include "gfx/RenderUniformBuffer.hpp"

#include "gfx/opengl/gl_defines.h"

#include <stdint.h>

namespace bolt {
namespace gfx {

class OpenglUniformBuffer : public RenderUniformBuffer {
public:
    OpenglUniformBuffer(size_t size, uint32_t bindPoint);
    virtual ~OpenglUniformBuffer();

    virtual void writeData(const void* src, size_t offset, size_t size) override;

    /// (re)binds this buffer to its bind point; needed when multiple buffers
    /// (e.g. one per drawable) share the same bind point and take turns occupying it
    void bind() const;

    GLuint bindPoint() const { return mBindPoint; }
    GLsizeiptr size() const { return mSize; }

private:
    GLuint mId;
    GLsizeiptr mSize;
    GLuint mBindPoint;
};

} // gfx
} // bolt
