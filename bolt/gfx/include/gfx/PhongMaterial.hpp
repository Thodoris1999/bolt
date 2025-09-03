#pragma once

#include "math/Vector.hpp"

namespace bolt {
namespace gfx {

struct PhongMaterial {
    math::Vector3f ambient;
    math::Vector3f diffuse;
    math::Vector3f specular;
    float shininess;
};

PhongMaterial randomBrightMaterial();

} // gfx
} // bolt
