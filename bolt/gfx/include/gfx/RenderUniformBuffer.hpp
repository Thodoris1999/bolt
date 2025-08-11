#pragma once

#include <cstddef>

namespace bolt {
namespace gfx {

class RenderUniformBuffer {
public:
    virtual void writeData(const void* src, size_t offset, size_t size) = 0;
};

} // gfx
} // bolt
