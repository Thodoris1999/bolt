#pragma once

#include "PhongDrawable.hpp"

namespace bolt {
namespace gfx {

class DrawableTriangle3d : public PhongDrawable {
public:
    DrawableTriangle3d();
    virtual ~DrawableTriangle3d() {}

    virtual const void* vertexData() const { return mVertices.data(); }
    virtual uint64_t vertexCount() const { return mVertices.size(); }
    virtual DrawOp drawOp() const { return BOLT_GFX_ARRAY; }

private:
    std::vector<PhongDrawableVertex> mVertices;
};

} // gfx
} // bolt
