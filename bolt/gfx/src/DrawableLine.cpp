#include "gfx/DrawableLine.hpp"

#include "math/math.h"

namespace bolt {
namespace gfx {

DrawableLine::DrawableLine(math::Vector3f start, math::Vector3f end) {
    mVertices = {
        {start, (start - end).normalize()},
        {end,   (end - start).normalize()},
    };

    mMtx.setRotation(DEG2RAD(0), 0, 0);
    mMtx.setTranslation(0, 0, 0);
}

} // gfx
} // bolt
