#pragma once

#include "PhongDrawable.hpp"

#include <vector>

namespace bolt {
namespace gfx {

class DrawableSpheroid : public PhongDrawable {
public:
    DrawableSpheroid(float radiusX, float radiusY, float radiusZ, int slices=16, int stacks=16);
    virtual ~DrawableSpheroid() {}

    virtual const void* vertexData() const { return mVertices.data(); }
    virtual uint64_t vertexCount() const { return mVertices.size(); }
    virtual DrawOp drawOp() const { return BOLT_GFX_INDEXED; }
    virtual const uint32_t* indexData() const { return mIndices.data(); }
    virtual uint64_t indexCount() const { return mIndices.size(); }

private:
    std::vector<PhongDrawableVertex> mVertices;
    std::vector<unsigned int> mIndices;
};

} // gfx
} // bolt
