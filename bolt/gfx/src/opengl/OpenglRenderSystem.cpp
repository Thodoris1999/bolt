#include "gfx/opengl/OpenglRenderSystem.hpp"
#include "gfx/opengl/OpenglProgram.hpp"
#include "gfx/opengl/OpenglTexture.hpp"

#include "gfx/opengl/gl_defines.h"

#include <memory>
#include <unordered_set>

namespace bolt {
namespace gfx {

static GLenum bolt2openglDtype(DataType type) {
    switch (type) {
        case BOLT_F32: return GL_FLOAT;
        case BOLT_I32: return GL_INT;
        default: {
            PANIC("Unknkown data type %d", static_cast<int>(type));
            return GL_FLOAT; // unknown type
        }
    }
}

static GLenum bolt2openglPrimitive(PrimitiveType type) {
    switch (type) {
        case BOLT_GFX_TRIANGLE: return GL_TRIANGLES;
        case BOLT_GFX_TRIANGLE_FAN: return GL_TRIANGLE_FAN;
        case BOLT_GFX_LINE: return GL_LINES;
        default: {
            PANIC("Unknkown primitive type %d", static_cast<int>(type));
            return GL_TRIANGLES; // unknown type
        }
    }
}

OpenglRenderSystem::~OpenglRenderSystem() {
    for (auto& entry : mLoadedPrograms) {
        delete entry.second;
    }
    for (auto& entry : mLoadedTextures) {
        delete entry.second;
    }
    for (auto* entry : mUniforms) {
        delete entry;
    }
    for (auto* d : mDrawables) {
        delete d;
    }
    free(mPushConstantBuffer);
}

OpenglDrawable::~OpenglDrawable() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void OpenglDrawable::load() {
    glGenBuffers(1, &VBO);
    if (drawable->indexData() != nullptr) {
        glGenBuffers(1, &EBO);
    }
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    int vertexSize = drawable->vertexSize();
    glBufferData(GL_ARRAY_BUFFER, drawable->vertexCount() * vertexSize, drawable->vertexData(), GL_DYNAMIC_DRAW);

    if (drawable->indexData() != nullptr) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        auto indexCount = drawable->indexCount();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(drawable->indexData()[0]), drawable->indexData(), GL_DYNAMIC_DRAW);
    }

    const auto* attrs = drawable->attributes();
    auto attrCount = drawable->attributeCount();
    for (int i = 0; i < attrCount; i++) {
        const VertexAttribute& attr = attrs[i];
        GLenum attrType = bolt2openglDtype(attr.dtype);
        glVertexAttribPointer(attr.index, attr.count, attrType, GL_FALSE, attr.stride, reinterpret_cast<const void*>(attr.offset));
        glEnableVertexAttribArray(attr.index);
    }

    glBindVertexArray(0);
}

OpenglRenderSystem::OpenglRenderSystem() {
    setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    mPushConstantBuffer = malloc(0); // just so that the free in the dtor is valid even if load is not called
}

void OpenglRenderSystem::setClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void OpenglRenderSystem::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void OpenglRenderSystem::addDrawable(Drawable* drawable) {
    OpenglDrawable* d = new OpenglDrawable();
    d->drawable = drawable;
    mDrawables.push_back(d);
}

RenderUniformBuffer* OpenglRenderSystem::addUniform(size_t size, uint32_t bindPoint) {
    RenderUniformBuffer* uni = new OpenglUniformBuffer(size, bindPoint);
    mUniforms.emplace_back(uni);
    return uni;
}

void OpenglRenderSystem::registerProgram(Drawable* d) {
    const csp::ProgramDescriptor& desc = d->programDescriptor();
    auto it = mLoadedPrograms.find(desc);

    if (it != mLoadedPrograms.end()) {
        d->setProgram(it->second);
    } else {
        RenderProgram* program = new OpenglProgram(&desc);
        mLoadedPrograms[desc] = program;
        d->setProgram(program);
    }
}

void OpenglRenderSystem::registerTexture(Drawable* d, const TextureDescriptor& desc, GLuint program) {
    auto it = mLoadedTextures.find(desc);

    if (it != mLoadedTextures.end()) {
        d->addTexture(it->second);
    } else {
        // TODO: current limitation does not allow same texture to be used in different samplers
        // change when ResourceManager is added
        OpenglTexture* texture = new OpenglTexture(desc.textureFile.c_str(), desc.binding);
        mLoadedTextures[desc] = texture;
        d->addTexture(texture);
    }
}

void OpenglRenderSystem::load() {
    size_t totalPushConstantSize = 0;
    size_t pushConstantUniformSize = 0;
    for (auto* d : mDrawables) {
        // calculate push constant needs
        uint32_t drawablePushConstantSize = d->drawable->pushConstantSize();
        if (pushConstantUniformSize < drawablePushConstantSize) {
            pushConstantUniformSize = drawablePushConstantSize;
        }
        totalPushConstantSize += drawablePushConstantSize;

        // load program
        registerProgram(d->drawable);
        auto openglProgram = static_cast<OpenglProgram*>(d->drawable->program());
        openglProgram->use();

        // load textures
        auto textureCount = d->drawable->textureCount();
        const TextureDescriptor* textures = d->drawable->textureDescriptors();
        for (uint32_t i = 0; i < textureCount; i++) {
            registerTexture(d->drawable, textures[i], openglProgram->id());
        }

        // load drawable buffers (vertices and indices)
        d->load();
    }
    glUseProgram(0);

    // allocate push constant CPU buffer and calculate drawable constants inside it
    mPushConstantBuffer = realloc(mPushConstantBuffer, totalPushConstantSize);
    size_t drawablePushConstantOffset = 0;
    for (auto* d : mDrawables) {
        // set push constant pointer
        uint32_t drawablePushConstantSize = d->drawable->pushConstantSize();
        d->drawable->setPushConstantData((uint8_t*)mPushConstantBuffer + drawablePushConstantOffset);
        drawablePushConstantOffset += drawablePushConstantSize;
    }

    /// create dummy uniform for push constants
    // find available binding point
    std::unordered_set<uint32_t> usedBindingPoints;
    for (size_t i = 0; i < mUniforms.size(); i++) {
        usedBindingPoints.insert(static_cast<OpenglUniformBuffer*>(mUniforms[i])->bindPoint());
    }
    uint32_t pushConstantBindPoint = 0;
    while (usedBindingPoints.find(pushConstantBindPoint) != usedBindingPoints.end()) {
        pushConstantBindPoint++;
    }
    mPushConstantUniform = new OpenglUniformBuffer(pushConstantUniformSize, pushConstantBindPoint);
    mUniforms.emplace_back(mPushConstantUniform);
    // set binding of push constant in programs
    for (auto& entry : mLoadedPrograms) {
        GLuint program = static_cast<OpenglProgram*>(entry.second)->id();
        GLuint blockIndex = glGetUniformBlockIndex(program, "PushConstants");
        if (blockIndex != GL_INVALID_INDEX) {
            glUniformBlockBinding(program, blockIndex, pushConstantBindPoint);
        } else {
            PANIC("Opengl: Failed to create auxiliary uniform for push constants");
        }
    }

    // create each drawable's own buffer for its set=2 uniform blocks
    // TODO: currently set=2 uniforms with the same bindings can collide with other sets' uniforms of the same binding
    // TODO: in the shader cross compiler, map these to a different binging. Alternatively remove explicit binding specifier on
    // TODO: the shader source and instead bind them manually here by name
    for (auto* d : mDrawables) {
        const csp::ProgramDescriptor& pd = d->drawable->programDescriptor();
        for (uint32_t i = 0; i < pd.uniform_count; i++) {
            const csp::UniformVar& uv = pd.uniform_vars[i];
            if (uv.set != 2) {
                continue;
            }
            if (uv.size == 0) {
                PANIC("Opengl: set=2 uniform block has zero size for \"%.*s\" — was csp regenerated with UniformVar.size support?", (int)uv.name.size(), uv.name.data());
            }

            auto uniform = std::make_unique<OpenglUniformBuffer>(uv.size, uv.binding);
            d->drawable->setUniformData(i, uniform.get());
            d->uniformBuffers.push_back(std::move(uniform));
        }

        d->drawable->onLoaded();
    }
}

void OpenglRenderSystem::renderFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto* d : mDrawables) {
        // configure program
        auto openglProgram = static_cast<OpenglProgram*>(d->drawable->program());
        openglProgram->use();

        // perform pre-draw operations
        d->drawable->onDraw();

        // rebind this drawable's set=2 uniform buffers to their shared bind points
        for (auto& uniform : d->uniformBuffers) {
            uniform->bind();
        }

        // set push constants
        mPushConstantUniform->writeData(d->drawable->pushConstantData(), 0, d->drawable->pushConstantSize());

        // select vertex array
        glBindVertexArray(d->VAO);

        // bind textures if they exist
        auto& textures = d->drawable->textures();
        for (size_t i = 0; i < textures.size(); i++) {
            auto* texture = static_cast<OpenglTexture*>(textures[i]);
            glActiveTexture(GL_TEXTURE0 + texture->binding());
            glBindTexture(GL_TEXTURE_2D, texture->id());
            texture->bind();
        }

        // perform draw call
        GLenum mode = bolt2openglPrimitive(d->drawable->primitiveType());
        switch (d->drawable->drawOp()) {
        case BOLT_GFX_ARRAY:
            glDrawArrays(mode, 0, d->drawable->vertexCount());
            break;
        case BOLT_GFX_INDEXED:
            glDrawElements(mode, d->drawable->indexCount(), GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
            break;
        default:
            PANIC("Invalid draw command %d", static_cast<int>(d->drawable->drawOp()));
        }

        // unbind everything
        glBindVertexArray(0);
        glUseProgram(0);
        if (d->drawable->textureCount() > 0) {
            glActiveTexture(GL_TEXTURE0);
        }
    }
}

} // gfx
} // bolt
