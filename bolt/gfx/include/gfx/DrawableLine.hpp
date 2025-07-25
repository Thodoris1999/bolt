#pragma once

#include "PhongDrawable.hpp"

#include <vector>

namespace bolt {
namespace gfx {

class DrawableLine : public PhongDrawable {
public:
    DrawableLine(math::Vector3f start, math::Vector3f end);

    virtual const void* vertexData() const { return mVertices.data(); }
    virtual uint64_t vertexCount() const { return mVertices.size(); }
    virtual DrawOp drawOp() const { return BOLT_GFX_ARRAY; }
    virtual PrimitiveType primitiveType() const { return BOLT_GFX_LINE; }

private:
    std::vector<PhongDrawableVertex> mVertices;
};

} // gfx
} // bolt
