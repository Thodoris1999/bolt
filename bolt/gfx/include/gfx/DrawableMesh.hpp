#pragma once

#include "Drawable.hpp"

#include "math/Vector.hpp"

#include <vector>

namespace bolt {
namespace gfx {

struct MeshVertex {
    math::Vector3f position;
    math::Vector3f normal;
    math::Vector2f texCoo;
};

class DrawableMesh : public Drawable3d {
public:
    DrawableMesh(std::vector<MeshVertex> vertices,
                           std::vector<unsigned int> indices,
                           std::vector<TextureDescriptor> textures);

    virtual const VertexAttribute* attributes() const override;
    virtual int attributeCount() const override;
    virtual const ProgramDescriptor& programDescriptor() const override;
    virtual const void* vertexData() const override { return mVertices.data(); }
    virtual uint64_t vertexCount() const override { return mVertices.size(); }
    virtual DrawOp drawOp() const override { return BOLT_GFX_INDEXED; }
    virtual const uint32_t* indexData() const override { return mIndices.data(); }
    virtual uint64_t indexCount() const override { return mIndices.size(); }
    virtual const TextureDescriptor* textureDescriptors() const override { return mTextureDesc.data(); }
    virtual int textureCount() const override { return mTextureDesc.size(); }

private:
    std::vector<MeshVertex> mVertices;
    std::vector<unsigned int> mIndices;
    std::vector<TextureDescriptor> mTextureDesc;
};

} // gfx
} // bolt
