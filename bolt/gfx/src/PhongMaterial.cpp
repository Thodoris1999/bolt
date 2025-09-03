#include "gfx/PhongMaterial.hpp"

#include "util/common.h"

#include <cstdlib>

namespace bolt {
namespace gfx {

PhongMaterial randomBrightMaterial() {
    PhongMaterial ret;

    math::Vector3f baseColor;
    do {
        baseColor.x = static_cast<float>(rand()) / RAND_MAX;
        baseColor.y = static_cast<float>(rand()) / RAND_MAX;
        baseColor.z = static_cast<float>(rand()) / RAND_MAX;
    } while (baseColor.x + baseColor.y + baseColor.z < 0.8f); // ensure not too dark

    float ambientFactor  = 0.3f + static_cast<float>(rand()) / RAND_MAX * 0.4f;  // 0.2 - 0.7
    float diffuseFactor  = 0.5f + static_cast<float>(rand()) / RAND_MAX * 0.5f;  // 0.5 - 1.0
    float specularFactor = 0.3f + static_cast<float>(rand()) / RAND_MAX * 0.7f;  // 0.3 - 1.0

    ret.ambient  = baseColor * ambientFactor;
    ret.diffuse  = baseColor * diffuseFactor;
    ret.specular = baseColor * specularFactor;

    ret.shininess = 32.0f + static_cast<float>(rand()) / RAND_MAX * 224.0f;

    return ret;
}

} // gfx
} // bolt
