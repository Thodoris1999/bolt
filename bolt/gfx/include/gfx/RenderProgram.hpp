#pragma once

#include "math/Vector.hpp"
#include "math/Matrix.hpp"
#include "Color.hpp"

#include <csp/csp.hpp>

namespace bolt {
namespace gfx {

/**
 * Interface over rendering API linked programs aka scripts aka pipelines
 */
class RenderProgram {
public:
    RenderProgram(const csp::ProgramDescriptor* pd) : mDescriptor(pd) {}
    virtual ~RenderProgram() {}
    /**
     * \name Functions for setting small shader data. They are defined by push constant blocks in the shader.
     *  The ID is an index into structures mainted by mDescriptor, which the graphics API can use to set the shader data
     * \{
     */
    virtual void setFloat(uint32_t id, float value) = 0;
    virtual void setVec3(uint32_t id, const math::Vector3f& value) = 0;
    virtual void setVec4(uint32_t id, const math::Vector4f& value) = 0;

    // set matrix row-major (i.e. gets transposed in opengl)
    virtual void setMat4(uint32_t id, const math::Matrix44f& value) = 0;

    void setColor(uint32_t id, const Color& value) {
        // color has the same ABI
        setVec4(id, reinterpret_cast<const math::Vector4f&>(value));
    }
    /** \} */

protected:
    const csp::ProgramDescriptor* mDescriptor;
};

} // gfx
} // bolt

