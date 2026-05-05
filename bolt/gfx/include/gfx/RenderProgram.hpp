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

protected:
    const csp::ProgramDescriptor* mDescriptor;
};

} // gfx
} // bolt

