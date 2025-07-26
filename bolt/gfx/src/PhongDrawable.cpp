#include "gfx/PhongDrawable.hpp"

#include "gfx/common.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

namespace bolt {
namespace gfx {

// Vertex attribute layout:
// Attribute 0: position (3 floats) -> offset 0
// Attribute 1: normal (3 floats) -> offset 12
static const VertexAttribute PHONG_VTX_ATTR[] = {
    {0, offsetof(PhongDrawableVertex, pos), 3, BOLT_F32, sizeof(PhongDrawableVertex)}, // position
    {1, offsetof(PhongDrawableVertex, nrm), 3, BOLT_F32, sizeof(PhongDrawableVertex)}  // normal
};

static const ProgramDescriptor PHONG_PROG_DESC = {
    BOLT_GFX_RES("phong.vert"),
    BOLT_GFX_RES("phong.frag")
};

const VertexAttribute* PhongDrawable::attributes() const {
    return PHONG_VTX_ATTR;
}

int PhongDrawable::attributeCount() const {
    return ARRAY_SIZE(PHONG_VTX_ATTR);
}

const ProgramDescriptor& PhongDrawable::programDescriptor() const {
    return PHONG_PROG_DESC;
}

void PhongDrawable::onDraw() {
    this->Drawable3d::onDraw();

    mProgram->setColor("ambientColor", mAmbient);
    // Future: diffuse, maybe specular
}

} // gfx
} // bolt
