#pragma once

#include "math/Matrix.hpp"

#include <string>

namespace bolt {
namespace gfx {

/// Common interface for skinned mesh types (textured or flat-shaded) so animation code
/// can drive bone transforms without caring which shading path a given mesh took.
class SkinnedDrawable {
public:
    virtual ~SkinnedDrawable() = default;

    virtual bool hasBone(const std::string& name) const = 0;
    // Note: bone transform will be undefined before first set
    virtual void setBoneTransform(const std::string& name, math::Matrix44f tf) = 0;
};

} // gfx
} // bolt
