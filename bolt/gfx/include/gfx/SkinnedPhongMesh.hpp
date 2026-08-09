#pragma once

#include "Drawable.hpp"
#include "SkinnedDrawable.hpp"
#include "SkinnedMesh.hpp"

#include "math/Vector.hpp"
#include "math/Matrix.hpp"

#include <vector>
#include <array>
#include <string>
#include <unordered_map>

namespace bolt {
namespace gfx {

struct SkinnedPhongMeshVertex {
    math::Vector3f position;
    math::Vector3f normal;
    std::array<int32_t, MAX_VTX_WEIGHTS> boneIds{};
    std::array<float, MAX_VTX_WEIGHTS> weights{};
};

class SkinnedPhongMesh : public Drawable3d, public SkinnedDrawable {
public:
    SkinnedPhongMesh(std::vector<SkinnedPhongMeshVertex> vertices,
                      std::vector<unsigned int> indices,
                      std::unordered_map<std::string, int32_t> boneMapping);

    virtual const VertexAttribute* attributes() const override;
    virtual int attributeCount() const override;
    virtual const csp::ProgramDescriptor& programDescriptor() const override;
    virtual const void* vertexData() const override { return mVertices.data(); }
    virtual uint64_t vertexCount() const override { return mVertices.size(); }
    virtual DrawOp drawOp() const override { return BOLT_GFX_INDEXED; }
    virtual const uint32_t* indexData() const override { return mIndices.data(); }
    virtual uint64_t indexCount() const override { return mIndices.size(); }
    virtual void onLoaded() override { setPoseDefault(); }
    virtual void onDraw() override;

    size_t boneCount() const { return mBoneMapping.size(); }
    virtual bool hasBone(const std::string& name) const override { return mBoneMapping.find(name) != mBoneMapping.end(); }
    virtual void setBoneTransform(const std::string& name, math::Matrix44f tf) override;
    void setPoseDefault();

    math::Vector3f& ambient() { return mAmbient; }
    math::Vector3f& diffuse() { return mDiffuse; }
    void setAmbient(const math::Vector3f& color) { mAmbient = color; }
    void setDiffuse(const math::Vector3f& color) { mDiffuse = color; }
    void setSpecular(const math::Vector3f& color) { mSpecular = color; }
    void setShininess(float shininess) { mShininess = shininess; }

private:
    std::vector<SkinnedPhongMeshVertex> mVertices;
    std::vector<unsigned int> mIndices;
    std::unordered_map<std::string, int32_t> mBoneMapping;

    math::Vector3f mAmbient;
    math::Vector3f mDiffuse;
    math::Vector3f mSpecular;
    float mShininess;
};

} // gfx
} // bolt
