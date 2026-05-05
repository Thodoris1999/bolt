#pragma once

#include "csp/csp.hpp"

#include "math/Matrix.hpp"
#include "gfx/SceneNode.hpp"
#include "gfx/RenderProgram.hpp"
#include "gfx/RenderTexture.hpp"
#include "util/common.h"

#include <string>
#include <cstring>
#include <cstdint>

namespace bolt {
namespace gfx {

enum DataType {
    BOLT_F32    
};

inline int sizeOfType(DataType type) {
    switch (type) {
    case BOLT_F32:
        return sizeof(float);
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
    virtual int textureCount() const { return 0; }
    /// Perform per draw operations before drawing like setting uniforms. For example Drawable3d uses this to set the model matrix
    virtual void onDraw() {}

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
        for (uint32_t i = 0; i < pd.push_constant_count; i++) {
            const csp::PushConstantEntry& pc = pd.push_constants[i];
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

protected:
    // members initialized by the rendering engine, only valid after load has been called on the RenderSystem
    void* mPushConstantData;
    RenderProgram* mProgram;
    std::vector<RenderTexture*> mTextures;
};

class Drawable3d : public Drawable, public SceneNode {
public:
    Drawable3d();

    /// set the model matrix. Do not forget to call Drawable3d::onDraw() if you override this otherwise you lose 3D-ness!
    virtual void onDraw() override;
};

} // gfx
} // bolt
