#pragma once

#include "csp/csp.hpp"

#include "math/Matrix.hpp"
#include "gfx/SceneNode.hpp"
#include "gfx/RenderProgram.hpp"
#include "gfx/RenderTexture.hpp"
#include "gfx/RenderUniformBuffer.hpp"
#include "util/common.h"

#include <string>
#include <cstring>
#include <cstdint>
#include <vector>

namespace bolt {
namespace gfx {

enum DataType {
    BOLT_F32,
    BOLT_I32,
};

inline int sizeOfType(DataType type) {
    switch (type) {
    case BOLT_F32:
        return sizeof(float);
    case BOLT_I32:
        return sizeof(int32_t);
    default: {
        PANIC("Unknkown type %d", static_cast<int>(type));
        return 0; // unknown type
    }
    }
}

struct VertexAttribute {
    int index;
    int offset;
    int count;
    DataType dtype;
    int stride;
};

struct TextureDescriptor {
    /// path to texture image file
    std::string textureFile;
    /// binding of sampler uniform in shader
    uint32_t binding;

    bool operator==(const TextureDescriptor& other) const noexcept {
        return textureFile == other.textureFile &&
               binding == other.binding;
    }
};

/**
 * Vector of descriptors needs to be ordered based on the binding number
 */
using Material = std::vector<TextureDescriptor>;

struct MaterialHash {
private:
    template<typename T>
    static void hashCombine(std::size_t& seed, const T& value) {
        seed ^= std::hash<T>{}(value)
              + 0x9e3779b9
              + (seed << 6)
              + (seed >> 2);
    }

public:
    std::size_t operator()(const Material& material) const noexcept {
        std::size_t seed = 0;

        for (const auto& tex : material) {
            hashCombine(seed, tex.textureFile);
            hashCombine(seed, tex.binding);
        }

        return seed;
    }
};

enum DrawOp {
    BOLT_GFX_ARRAY,
    BOLT_GFX_INDEXED,
};

enum PrimitiveType {
    BOLT_GFX_TRIANGLE,
    BOLT_GFX_TRIANGLE_FAN,
    BOLT_GFX_LINE,
};

class Drawable {
public:
    virtual const VertexAttribute* attributes() const = 0;
    virtual int attributeCount() const = 0;
    virtual const csp::ProgramDescriptor& programDescriptor() const = 0;
    virtual const void* vertexData() const = 0;
    virtual uint64_t vertexCount() const = 0;
    virtual DrawOp drawOp() const = 0;
    virtual PrimitiveType primitiveType() const { return BOLT_GFX_TRIANGLE; };
    virtual const uint32_t* indexData() const { return nullptr; }
    virtual uint64_t indexCount() const { return 0; }
    virtual const TextureDescriptor* textureDescriptors() const { return nullptr; }
    virtual uint32_t textureCount() const { return 0; }
    /// Perform per draw operations before drawing like setting uniforms. For example Drawable3d uses this to set the model matrix
    virtual void onDraw() {}

    /// Called once by the RenderSystem right after this drawable has been loaded, i.e. right after
    /// it becomes safe to access the drawable's uniform or program via setUniform(), program()
    virtual void onLoaded() {}

    // Drawable pushConstantBuffer
    void setPushConstantData(void* data) { mPushConstantData = data; };
    void* pushConstantData() { return mPushConstantData; }
    const void* pushConstantData() const { return mPushConstantData; }

    // Drawable program
    void setProgram(RenderProgram* program) { mProgram = program; };
    RenderProgram* program() { return mProgram; }
    const RenderProgram* program() const { return mProgram; }

    // drawable textures
    const std::vector<RenderTexture*>& textures() { return mTextures; }
    void addTexture(RenderTexture* texture) { mTextures.push_back(texture); }

    // Compute size of one vertex in bytes
    int vertexSize() const {
        int total = 0;
        const VertexAttribute* attrs = attributes();
        for (int i = 0; i < attributeCount(); ++i) {
            total += attrs[i].count * sizeOfType(attrs[i].dtype);
        }
        return total;
    }

    // Compute required size of all push constants
    uint32_t pushConstantSize() const {
        uint32_t pushConstantSize = 0;
        const csp::ProgramDescriptor& pd = programDescriptor();
        for (uint32_t i = 0; i < pd.push_constant_range_count; i++) {
            const csp::PushConstantRange& pc = pd.push_constant_ranges[i];
            if (pc.offset + pc.size > pushConstantSize) {
                pushConstantSize = pc.offset + pc.size;
            }
        }
        return pushConstantSize;
    }

    void setPushConstant(uint32_t id, const void* data) {
        const csp::PushConstantEntry& pc = programDescriptor().push_constants[id];
        memcpy((uint8_t*)mPushConstantData + pc.offset, data, pc.size);
    }

    // Drawable set=2 uniform blocks. offset lets a caller update a sub-range of the block without re-uploading the whole thing.
    void setUniform(uint32_t id, const void* data, size_t size, size_t offset = 0) {
        const csp::UniformVar& uv = programDescriptor().uniform_vars[id];
        if (offset + size > uv.size) {
            PANIC("Drawable::setUniform: offset %zu + size %zu exceeds reflected uniform size %u for id %u (\"%.*s\")",
                offset, size, uv.size, id, (int)uv.name.size(), uv.name.data());
        }
        if (id >= mUniformData.size() || mUniformData[id] == nullptr) {
            PANIC("Drawable::setUniform: no backing storage for uniform id %u — was load() called, and is set==2?", id);
        }
        mUniformData[id]->writeData(data, offset, size);
    }

    void setUniformData(uint32_t id, RenderUniformBuffer* buffer) {
        if (id >= mUniformData.size()) {
            mUniformData.resize(id + 1, nullptr);
        }
        mUniformData[id] = buffer;
    }

protected:
    // members initialized by the rendering engine, only valid after load has been called on the RenderSystem
    void* mPushConstantData;
    RenderProgram* mProgram;
    std::vector<RenderTexture*> mTextures;
    std::vector<RenderUniformBuffer*> mUniformData;
};

class Drawable3d : public Drawable, public SceneNode {
public:
    Drawable3d();

    /// set the model matrix. Do not forget to call Drawable3d::onDraw() if you override this otherwise you lose 3D-ness!
    virtual void onDraw() override;
};

} // gfx
} // bolt
