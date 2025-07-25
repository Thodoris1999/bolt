#include "gfx/DrawableTriangle.hpp"
#include "math/math.h"

namespace bolt {
namespace gfx {

DrawableTriangle3d::DrawableTriangle3d() {
    mVertices = {
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{0.0f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    };

    mMtx.setRotation(DEG2RAD(0), 0, 0);
    mMtx.setTranslation(0, 0, 0);
}

} // gfx
} // bolt
