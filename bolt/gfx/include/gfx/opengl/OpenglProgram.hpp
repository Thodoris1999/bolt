#pragma once

#include "gfx/RenderProgram.hpp"

#include "glad/glad.h"

namespace bolt {
namespace gfx {

class OpenglProgram final : public RenderProgram {
public:
    OpenglProgram(const char* vtxShader, const char* fragShader);
    virtual ~OpenglProgram() { glDeleteProgram(mId); }

    /// @beginGetters
    unsigned int id() const { return mId; }
    /// @endGetters

    virtual void use() override {
        glUseProgram(mId);
    }

    /**
     * \name Functions for setting shader uniforms
     * \{
     */
    virtual void setVec4(const char* name, const math::Vector4f& value) override {
        glUniform4fv(glGetUniformLocation(mId, name), 1, (const GLfloat*)&value);
    }

    // set matrix row-major (i.e. gets transposed in opengl)
    virtual void setMat4(const char* name, const math::Matrix44f& value) override {
        glUniformMatrix4fv(glGetUniformLocation(mId, name), 1, GL_TRUE, (const GLfloat*)&value);
    }
    /** \} */

private:
    GLuint mId;
};

} // gfx
} // bolt
