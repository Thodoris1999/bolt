#include "gfx/Drawable.hpp"

#include "gfx/gl_defines.h"

namespace bolt {
namespace gfx {

Drawable3d::Drawable3d() {
    mMtx.makeHomogeneous();
}

void Drawable3d::onDraw() {
    mShader.use();
    mShader.setMat4("model", mMtx);
}

} // gfx
} // bolt
