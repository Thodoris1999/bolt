#pragma once

namespace bolt {
namespace gfx {

struct DirLightParams {
    math::Vector3f direction;   float pad0;

    math::Vector3f ambient;   float pad1;
    math::Vector3f diffuse;   float pad2;
    math::Vector3f specular;   float pad3;

    DirLightParams() = default;
    DirLightParams(const math::Vector3f& dir,
                   const math::Vector3f& amb,
                   const math::Vector3f& diff,
                   const math::Vector3f& spec)
        : direction(dir), pad0(0.0f),
          ambient(amb), pad1(0.0f),
          diffuse(diff), pad2(0.0f),
          specular(spec), pad3(0.0f) {}
};

} // gfx
} // bolt
