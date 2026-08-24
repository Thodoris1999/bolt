#pragma once

#include "gfx/RenderSystem.hpp"

#include "gfx/opengl/gl_defines.h"
#include "gfx/opengl/OpenglUniformBuffer.hpp"

#include <memory>
#include <unordered_map>

namespace bolt {
namespace gfx {

struct OpenglDrawable {
    Drawable* drawable = nullptr;
    GLuint VBO = 0;
    GLuint EBO = 0;
    GLuint VAO = 0;
    // one per set=2 uniform block declared by this drawable's program
    std::vector<std::unique_ptr<OpenglUniformBuffer>> uniformBuffers;
    // this drawable's own push-constant scratch storage, sized to its program's
    // pushConstantSize() once loaded
    std::vector<uint8_t> pushConstantData;

    ~OpenglDrawable();

    void load();
};

class OpenglRenderSystem final : public RenderSystem {
public:
    OpenglRenderSystem();
    virtual ~OpenglRenderSystem();
    virtual void setClearColor(float r, float b, float g, float a) override;
    virtual void setViewport(int x, int y, int width, int height) override;
    virtual void addDrawable(Drawable* drawable) override;
    virtual void removeDrawable(Drawable* drawable) override;
    virtual RenderUniformBuffer* addUniform(size_t size, uint32_t bindPoint) override;
    virtual void load() override;
    virtual void renderFrame() override;

private:
    std::unordered_map<Drawable*, std::unique_ptr<OpenglDrawable>> mDrawables;

    // TODO: combine into a ResourceManager
    struct ProgramDescriptorHash {
        std::size_t operator()(const csp::ProgramDescriptor& p) const {
            std::size_t seed = 0;
            std::hash<std::string_view> hasher;

            for (uint32_t i = 0; i < p.source_count; ++i) {
                const auto& src = p.ogl_sources[i].filename;
                std::size_t h = hasher(src);

                // hash combine (boost-style)
                seed ^= h + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            }

            return seed;
        }
    };
    struct ProgramDescriptorEqual {
        bool operator()(const csp::ProgramDescriptor& lhs, const csp::ProgramDescriptor& rhs) const {
            if (lhs.source_count != rhs.source_count) {
                return false;
            }
            // filenames must match in order
            for (uint32_t i = 0; i < lhs.source_count; ++i) {
                if (lhs.ogl_sources[i].filename != rhs.ogl_sources[i].filename) {
                    return false;
                }
            }

            return true;
        }
    };
    void registerProgram(Drawable* d, uint32_t pushConstantBindPoint);
    std::unordered_map<csp::ProgramDescriptor, RenderProgram*, ProgramDescriptorHash, ProgramDescriptorEqual> mLoadedPrograms;

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

    // GL has no descriptor sets, so a set=2 block and a set=0 block can be cross compiled onto
    // the same declared binding. Each program's set=2 blocks are therefore rebound to bind points
    // of our own choosing, remembered here (program id -> set=2 binding -> bind point) so that a
    // drawable loaded by a later load() reuses the point its program was already bound to.
    std::unordered_map<GLuint, std::unordered_map<uint32_t, uint32_t>> mSet2BindPoints;

    static constexpr uint32_t INVALID_BIND_POINT = ~0u;
    uint32_t mPushConstantBindPoint = INVALID_BIND_POINT;
    OpenglUniformBuffer* mPushConstantUniform = nullptr;
};

} // gfx
} // bolt
