#pragma once

#include "math/Vector.hpp"
#include "math/Matrix.hpp"
#include "Color.hpp"

namespace bolt {
namespace gfx {

/**
 * Interface over rendering API linked programs aka scripts aka pipelines
 */
class RenderProgram {
public:
    virtual ~RenderProgram() {}
    // activate shader
    virtual void use() = 0;

    /**
     * \name Functions for setting shader uniforms
     * \{
     */
    virtual void setVec4(const char* name, const math::Vector4f& value) = 0;

    // set matrix row-major (i.e. gets transposed in opengl)
    virtual void setMat4(const char* name, const math::Matrix44f& value) = 0;

    void setColor(const char* name, const Color& value) {
        // color has the same ABI
        setVec4(name, reinterpret_cast<const math::Vector4f&>(value));
    }
    /** \} */
};

} // gfx
} // bolt

