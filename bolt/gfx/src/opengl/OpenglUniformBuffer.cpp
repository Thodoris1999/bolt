#include "gfx/opengl/OpenglUniformBuffer.hpp"

namespace bolt {
namespace gfx {

OpenglUniformBuffer::OpenglUniformBuffer(size_t size, uint32_t bindPoint) : mSize(size), mBindPoint(bindPoint) {
    glGenBuffers(1, &mId);
    glBindBuffer(GL_UNIFORM_BUFFER, mId);
    glBufferData(GL_UNIFORM_BUFFER, mSize, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glCheckError();

    // bind buffer to binding point
    glBindBufferRange(GL_UNIFORM_BUFFER, mBindPoint, mId, 0, mSize);
}

OpenglUniformBuffer::~OpenglUniformBuffer() {
    if (mId != 0) {
        glDeleteBuffers(1, &mId);
        mId = 0;
    }
}

void OpenglUniformBuffer::writeData(const void* src, size_t offset, size_t size) {
    glBindBuffer(GL_UNIFORM_BUFFER, mId);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, src);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

} // gfx
} // bolt
