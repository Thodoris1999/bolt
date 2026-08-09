#pragma once

#include "Drawable.hpp"

#include "math/Vector.hpp"
#include "math/Matrix.hpp"

#include <vector>
#include <array>
#include <string>
#include <unordered_map>

#define MAX_VTX_WEIGHTS 4
#define MAX_BONES 96

namespace bolt {
namespace gfx {

struct SkinnedMeshVertex {
    math::Vector3f position;
    math::Vector3f normal;
    math::Vector2f texCoo;
    math::Vector3f tangent;
    math::Vector3f bitangent;
    std::array<int32_t, MAX_VTX_WEIGHTS> boneIds{};
    std::array<float, MAX_VTX_WEIGHTS> weights{};
};

class SkinnedMesh : public Drawable3d {
public:
    SkinnedMesh(std::vector<SkinnedMeshVertex> vertices,
                           std::vector<unsigned int> indices,
                           std::vector<TextureDescriptor> textures,
                           std::unordered_map<std::string, int32_t> boneMapping);

    virtual const VertexAttribute* attributes() const override;
    virtual int attributeCount() const override;
    virtual const csp::ProgramDescriptor& programDescriptor() const override;
    virtual const void* vertexData() const override { return mVertices.data(); }
    virtual uint64_t vertexCount() const override { return mVertices.size(); }
    virtual DrawOp drawOp() const override { return BOLT_GFX_INDEXED; }
    virtual const uint32_t* indexData() const override { return mIndices.data(); }
    virtual uint64_t indexCount() const override { return mIndices.size(); }
    virtual const TextureDescriptor* textureDescriptors() const override { return mTextureDesc.data(); }
    virtual uint32_t textureCount() const override { return mTextureDesc.size(); }
    virtual void onLoaded() override { setPoseDefault(); }

    size_t boneCount() const { return mBoneMapping.size(); }
    bool hasBone(const std::string& name) const { return mBoneMapping.find(name) != mBoneMapping.end(); }

    // Note: bone transform will be undefined before first set
    void setBoneTransform(const std::string& name, math::Matrix44f tf);

    // Sets every bone to identity, reproducing the mesh's authored bind pose (T-pose).
    // Call after RenderSystem::load() has run, otherwise setUniform() will PANIC.
    void setPoseDefault();

private:
    std::vector<SkinnedMeshVertex> mVertices;
    std::vector<unsigned int> mIndices;
    std::vector<TextureDescriptor> mTextureDesc;
    std::unordered_map<std::string, int32_t> mBoneMapping;
};

} // gfx
} // bolt
