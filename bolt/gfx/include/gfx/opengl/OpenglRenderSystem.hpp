#pragma once

#include "gfx/RenderSystem.hpp"

#include "gfx/opengl/gl_defines.h"
#include "gfx/opengl/OpenglUniformBuffer.hpp"

#include <unordered_map>

namespace bolt {
namespace gfx {

struct OpenglDrawable {
    Drawable* drawable;
    GLuint VBO;
    GLuint EBO;
    GLuint VAO;

    void load();
};

class OpenglRenderSystem final : public RenderSystem {
public:
    OpenglRenderSystem();
    virtual ~OpenglRenderSystem();
    virtual void setViewport(int x, int y, int width, int height) override;
    virtual void addDrawable(Drawable* drawable) override;
    virtual void load() override;
    virtual void renderFrame(const Camera& camera) override;

private:
    std::vector<OpenglDrawable> mDrawables;

    // TODO: combine into a ResourceManager
    struct ProgramDescriptorHash {
        std::size_t operator()(const ProgramDescriptor& p) const {
            return std::hash<std::string>()(p.vertShader) ^ (std::hash<std::string>()(p.fragShader) << 1);
        }
    };
    struct ProgramDescriptorEqual {
        bool operator()(const ProgramDescriptor& lhs, const ProgramDescriptor& rhs) const {
            return lhs.vertShader == rhs.vertShader &&
                   lhs.fragShader == rhs.fragShader;
        }
    };
    void registerProgram(Drawable* d);
    std::unordered_map<ProgramDescriptor, RenderProgram*, ProgramDescriptorHash, ProgramDescriptorEqual> mLoadedPrograms;

    struct TextureDescriptorHash {
        std::size_t operator()(const TextureDescriptor& t) const {
            return std::hash<std::string>()(t.textureFile);
        }
    };
    struct TextureDescriptorEqual {
        bool operator()(const TextureDescriptor& lhs, const TextureDescriptor& rhs) const {
            return lhs.textureFile == rhs.textureFile;
        }
    };
    void registerTexture(Drawable* d, const TextureDescriptor& desc, GLuint program);
    std::unordered_map<TextureDescriptor, RenderTexture*, TextureDescriptorHash, TextureDescriptorEqual> mLoadedTextures;
};

} // gfx
} // bolt
