#include "gfx/opengl/OpenglRenderSystem.hpp"
#include "gfx/opengl/OpenglProgram.hpp"
#include "gfx/opengl/OpenglTexture.hpp"

#include "gfx/opengl/gl_defines.h"

namespace bolt {
namespace gfx {

static GLenum bolt2openglDtype(DataType type) {
    switch (type) {
        case BOLT_F32: return GL_FLOAT;
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
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void OpenglRenderSystem::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void OpenglRenderSystem::addDrawable(Drawable* drawable) {
    OpenglDrawable d;
    d.drawable = drawable;
    mDrawables.push_back(d);
}

void OpenglRenderSystem::registerProgram(Drawable* d) {
    const ProgramDescriptor& desc = d->programDescriptor();
    auto it = mLoadedPrograms.find(desc);

    if (it != mLoadedPrograms.end()) {
        d->setProgram(it->second);
    } else {
        RenderProgram* program = new OpenglProgram(desc.vertShader.c_str(), desc.fragShader.c_str());
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
        GLint samplerLoc = glGetUniformLocation(program, desc.samplerName.c_str());
        OpenglTexture* texture = new OpenglTexture(desc.textureFile.c_str(), samplerLoc);
        mLoadedTextures[desc] = texture;
        d->addTexture(texture);
    }
}

struct CameraData {
    math::Matrix44f projection;
    math::Matrix44f view;
};

void OpenglRenderSystem::load() {
    // create uniform blocks
    // camera view and projection matrices
    mUniforms.emplace_back(new OpenglUniformBuffer(sizeof(CameraData), 0));
    // camera position
    mUniforms.emplace_back(new OpenglUniformBuffer(sizeof(math::Vector3f), 1));
    // directional light
    mUniforms.emplace_back(new OpenglUniformBuffer(sizeof(DirLightParams), 2));
    
    for (auto& d : mDrawables) {
        // load program
        registerProgram(d.drawable);
        auto openglProgram = static_cast<OpenglProgram*>(d.drawable->program());
        openglProgram->use();

        // load textures
        auto textureCount = d.drawable->textureCount();
        const TextureDescriptor* textures = d.drawable->textureDescriptors();
        for (int i = 0; i < textureCount; i++) {
            registerTexture(d.drawable, textures[i], openglProgram->id());
        }

        // load drawable buffers (vertices and indices)
        d.load();
    }
    glUseProgram(0);
}

void OpenglRenderSystem::renderFrame(const Camera& camera) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set uniforms blocks
    // it would be nice to allow flexibility for custom uniform block bindings
    // camera matrices located at binding point 0
    CameraData cameraData;
    cameraData.projection = camera.getProjection();
    cameraData.view = camera.getView();
    mUniforms[0]->writeData(&cameraData, 0, sizeof(cameraData));
    // camera position located at binding point 1
    math::Vector3f camPos = camera.worldPos();
    mUniforms[1]->writeData(&camPos, 0, sizeof(math::Vector3f));
    // directional light located at binding point 2
    // (set by SceneManager API)

    for (const auto& d : mDrawables) {
        // configure program
        d.drawable->program()->use();

        // perform pre-draw operations
        d.drawable->onDraw();

        // select vertex array
        glBindVertexArray(d.VAO);

        // bind textures if they exist
        auto& textures = d.drawable->textures();
        for (int i = 0; i < textures.size(); i++) {
            auto* texture = static_cast<OpenglTexture*>(textures[i]);
            glActiveTexture(GL_TEXTURE0 + i);
            glUniform1i(texture->samplerLocation(), i);
            texture->bind();
        }

        // perform draw call
        GLenum mode = bolt2openglPrimitive(d.drawable->primitiveType());
        switch (d.drawable->drawOp()) {
        case BOLT_GFX_ARRAY:
            glDrawArrays(mode, 0, d.drawable->vertexCount());
            break;
        case BOLT_GFX_INDEXED:
            glDrawElements(mode, d.drawable->indexCount(), GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
            break;
        default:
            PANIC("Invalid draw command %d", static_cast<int>(d.drawable->drawOp()));
        }

        // unbind everything
        glBindVertexArray(0);
        glUseProgram(0);
        if (d.drawable->textureCount() > 0) {
            glActiveTexture(GL_TEXTURE0);
        }
    }
}

} // gfx
} // bolt
