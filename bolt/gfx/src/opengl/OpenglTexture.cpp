#include "gfx/opengl/OpenglTexture.hpp"

#include "util/common.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace bolt {
namespace gfx {

OpenglTexture::OpenglTexture(const char* textureFile, GLint samplerLocation) : mSamplerLocation(samplerLocation) {
    glGenTextures(1, &mId);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(textureFile, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, mId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        PANIC("Failed to load texture from path: %s", textureFile);
    }
}

void OpenglTexture::bind() {
    glBindTexture(GL_TEXTURE_2D, mId);
}

} // gfx
} // bolt
