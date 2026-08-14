#pragma once

#include "FlatDrawable.hpp"

#include <vector>

namespace bolt {
namespace gfx {

class DrawableWireframe : public FlatDrawable {
public:
    DrawableWireframe(std::vector<FlatDrawableVertex> vertices,
                       std::vector<unsigned int> indices) : mVertices(vertices), mIndices(indices) {}
    virtual ~DrawableWireframe() {}

    virtual const void* vertexData() const override { return mVertices.data(); }
    virtual uint64_t vertexCount() const override { return mVertices.size(); }
    virtual DrawOp drawOp() const override { return BOLT_GFX_INDEXED; }
    virtual const uint32_t* indexData() const override { return mIndices.data(); }
    virtual uint64_t indexCount() const override { return mIndices.size(); }
    virtual PrimitiveType primitiveType() const override { return BOLT_GFX_LINE; }

private:
    std::vector<FlatDrawableVertex> mVertices;
    std::vector<uint32_t> mIndices;
};

} // gfx
} // bolt
