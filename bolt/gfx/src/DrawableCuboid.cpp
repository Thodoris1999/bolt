#include "gfx/DrawableCuboid.hpp"
#include "math/math.h"

namespace bolt {
namespace gfx {

DrawableCuboid::DrawableCuboid(float sizeX, float sizeY, float sizeZ) {
    float hx = sizeX / 2;
    float hy = sizeY / 2;
    float hz = sizeZ / 2;

    mVertices = {
        {{-hx, -hy,  hz}, {0.0f, 0.0f, 1.0f}},
        {{hx, -hy,  hz}, {0.0f, 0.0f, 1.0f}},
        {{hx,  hy,  hz}, {0.0f, 0.0f, 1.0f}},
        {{-hx, -hy,  hz}, {0.0f, 0.0f, 1.0f}},
        {{-hx,  hy,  hz}, {0.0f, 0.0f, 1.0f}},
        {{hx,  hy,  hz}, {0.0f, 0.0f, 1.0f}},

        {{-hx, -hy, -hz}, {0.0f, 0.0f, -1.0f}},
        {{hx, -hy, -hz}, {0.0f, 0.0f, -1.0f}},
        {{hx,  hy, -hz}, {0.0f, 0.0f, -1.0f}},
        {{-hx, -hy, -hz}, {0.0f, 0.0f, -1.0f}},
        {{-hx,  hy, -hz}, {0.0f, 0.0f, -1.0f}},
        {{hx,  hy, -hz}, {0.0f, 0.0f, -1.0f}},

        {{-hx,  hy, -hz}, {0.0f, 1.0f, 0.0f}},
        {{hx,  hy, -hz}, {0.0f, 1.0f, 0.0f}},
        {{hx,  hy,  hz}, {0.0f, 1.0f, 0.0f}},
        {{-hx,  hy, -hz}, {0.0f, 1.0f, 0.0f}},
        {{-hx,  hy,  hz}, {0.0f, 1.0f, 0.0f}},
        {{hx,  hy,  hz}, {0.0f, 1.0f, 0.0f}},

        {{-hx, -hy, -hz}, {0.0f, -1.0f, 0.0f}},
        {{hx, -hy, -hz}, {0.0f, -1.0f, 0.0f}},
        {{hx, -hy,  hz}, {0.0f, -1.0f, 0.0f}},
        {{-hx, -hy, -hz}, {0.0f, -1.0f, 0.0f}},
        {{-hx, -hy,  hz}, {0.0f, -1.0f, 0.0f}},
        {{hx, -hy,  hz}, {0.0f, -1.0f, 0.0f}},

        {{hx, -hy, -hz}, {1.0f, 0.0f, 0.0f}},
        {{hx,  hy, -hz}, {1.0f, 0.0f, 0.0f}},
        {{hx,  hy,  hz}, {1.0f, 0.0f, 0.0f}},
        {{hx, -hy, -hz}, {1.0f, 0.0f, 0.0f}},
        {{hx, -hy,  hz}, {1.0f, 0.0f, 0.0f}},
        {{hx,  hy,  hz}, {1.0f, 0.0f, 0.0f}},

        {{-hx, -hy, -hz}, {-1.0f, 0.0f, 0.0f}},
        {{-hx,  hy, -hz}, {-1.0f, 0.0f, 0.0f}},
        {{-hx,  hy,  hz}, {-1.0f, 0.0f, 0.0f}},
        {{-hx, -hy, -hz}, {-1.0f, 0.0f, 0.0f}},
        {{-hx, -hy,  hz}, {-1.0f, 0.0f, 0.0f}},
        {{-hx,  hy,  hz}, {-1.0f, 0.0f, 0.0f}},
    };

    mMtx.setRotation(DEG2RAD(0), 0, 0);
    mMtx.setTranslation(0, 0, 0);
}

} // gfx
} // bolt
