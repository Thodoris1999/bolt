#pragma once

#include "gfx/RenderProgram.hpp"

#include "glad/glad.h"

namespace bolt {
namespace gfx {

class OpenglProgram final : public RenderProgram {
public:
    OpenglProgram(const csp::ProgramDescriptor* pd);
    virtual ~OpenglProgram() { glDeleteProgram(mId); }

    /// @beginGetters
    unsigned int id() const { return mId; }
    /// @endGetters

    void use() {
        glUseProgram(mId);
    }

private:
    GLuint mId;
};

} // gfx
} // bolt
