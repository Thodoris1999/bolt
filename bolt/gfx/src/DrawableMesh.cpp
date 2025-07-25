#include "gfx/DrawableMesh.hpp"
#include "gfx/common.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

namespace bolt {
namespace gfx {

DrawableMesh::DrawableMesh(std::vector<MeshVertex> vertices,
                           std::vector<unsigned int> indices,
                           std::vector<TextureDescriptor> textures) :
                           mVertices(vertices), mIndices(indices), mTextureDesc(textures) {
}

// Vertex attribute layout:
// Attribute 0: position (3 floats) -> offset 0
// Attribute 1: normal (3 floats) -> offset 12
// Attribute 2: tex coords (2 floats) -> offset 20
static const VertexAttribute MESH_VTX_ATTR[] = {
    {0, offsetof(MeshVertex, position), 3, BOLT_F32, sizeof(MeshVertex)}, // position
    {1, offsetof(MeshVertex, normal), 3, BOLT_F32, sizeof(MeshVertex)},   // normal
    {2, offsetof(MeshVertex, texCoo), 2, BOLT_F32, sizeof(MeshVertex)}    // texture coordinates
};

static const ProgramDescriptor MESH_PROG_DESC = {
    BOLT_GFX_RES("textured.vert"),
    BOLT_GFX_RES("textured.frag")
};

const VertexAttribute* DrawableMesh::attributes() const {
    return MESH_VTX_ATTR;
}

int DrawableMesh::attributeCount() const {
    return ARRAY_SIZE(MESH_VTX_ATTR);
}

const ProgramDescriptor& DrawableMesh::programDescriptor() const {
    return MESH_PROG_DESC;
}

} // gfx
} // bolt
