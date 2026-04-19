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

    /**
     * \name Functions for setting shader uniforms
     * \{
     */
    virtual void setFloat(uint32_t id, float value) override {
        glUniform1f(glGetUniformLocation(mId, mDescriptor->push_constants[id].name.data()), (const GLfloat)value);
    }
    virtual void setVec3(uint32_t id, const math::Vector3f& value) override {
        glUniform3fv(glGetUniformLocation(mId, mDescriptor->push_constants[id].name.data()), 1, (const GLfloat*)&value);
    }
    virtual void setVec4(uint32_t id, const math::Vector4f& value) override {
        glUniform4fv(glGetUniformLocation(mId, mDescriptor->push_constants[id].name.data()), 1, (const GLfloat*)&value);
    }

    // set matrix row-major (i.e. gets transposed in opengl)
    virtual void setMat4(uint32_t id, const math::Matrix44f& value) override {
        glUniformMatrix4fv(glGetUniformLocation(mId, mDescriptor->push_constants[id].name.data()), 1, GL_TRUE, (const GLfloat*)&value);
    }
    /** \} */

private:
    GLuint mId;
};

} // gfx
} // bolt
