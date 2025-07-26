#include "gfx/FlatDrawable.hpp"

#include "gfx/common.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

namespace bolt {
namespace gfx {

// Vertex attribute layout:
// Attribute 0: position (3 floats) -> offset 0
static const VertexAttribute FLAT_VTX_ATTR[] = {
    {0, offsetof(FlatDrawableVertex, pos), 3, BOLT_F32, sizeof(FlatDrawableVertex)}, // position
};

static const ProgramDescriptor FLAT_PROG_DESC = {
    BOLT_GFX_RES("flat.vert"),
    BOLT_GFX_RES("flat.frag")
};

const VertexAttribute* FlatDrawable::attributes() const {
    return FLAT_VTX_ATTR;
}

int FlatDrawable::attributeCount() const {
    return ARRAY_SIZE(FLAT_VTX_ATTR);
}

const ProgramDescriptor& FlatDrawable::programDescriptor() const {
    return FLAT_PROG_DESC;
}

void FlatDrawable::onDraw() {
    this->Drawable3d::onDraw();

    mProgram->setColor("color", mColor);
}

} // gfx
} // bolt
