#pragma once

#include "math/Vector.hpp"

namespace bolt {
namespace gfx {

struct PhongMaterial {
    math::Vector3f ambient;  float pad1;
    math::Vector3f diffuse;  float pad2;
    math::Vector3f specular;  float pad3;
    float shininess;
};

PhongMaterial randomBrightMaterial();

} // gfx
} // bolt
