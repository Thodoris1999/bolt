#pragma once

#include "math/Matrix.hpp"
#include "gfx/SceneNode.hpp"
#include "gfx/RenderProgram.hpp"
#include "gfx/RenderTexture.hpp"
#include "util/common.h"

#include <string>
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

struct ProgramDescriptor {
    /// path to vertex shader
    std::string vertShader;
    /// path to fragment shader
    std::string fragShader;
};

struct TextureDescriptor {
    /// path to texture image file
    std::string textureFile;
    /// name of sampler uniform in shader
    std::string samplerName;
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
    virtual const ProgramDescriptor& programDescriptor() const = 0;
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

protected:
    RenderProgram* mProgram;
    std::vector<RenderTexture*> mTextures;
};

class Drawable3d : public Drawable, public SceneNode {
public:
    Drawable3d();

    /// set the model matrix. Do not forget to call Drawable3d::onDraw() if you override this otherwise you lose 3D-ness!
    virtual void onDraw() override { mProgram->setMat4("model", worldMtx()); }
};

} // gfx
} // bolt
