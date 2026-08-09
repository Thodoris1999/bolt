#include "gfx/AssimpModel.hpp"
#include "gfx/AssimpAnimation.hpp"
#include "gfx/TexturedMesh.hpp"
#include "gfx/PhongMesh.hpp"
#include "gfx/SkinnedMesh.hpp"
#include "gfx/SkinnedPhongMesh.hpp"

#include "util/common.h"
#include "gfx/common.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <unordered_map>

namespace bolt {
namespace gfx {

static math::Matrix44f toMatrix44f(const aiMatrix4x4& m) {
    math::Matrix44f result;
    result(0,0) = m.a1; result(0,1) = m.a2; result(0,2) = m.a3; result(0,3) = m.a4;
    result(1,0) = m.b1; result(1,1) = m.b2; result(1,2) = m.b3; result(1,3) = m.b4;
    result(2,0) = m.c1; result(2,1) = m.c2; result(2,2) = m.c3; result(2,3) = m.c4;
    result(3,0) = m.d1; result(3,1) = m.d2; result(3,2) = m.d3; result(3,3) = m.d4;
    return result;
}

static std::vector<unsigned int> extractIndices(const aiMesh* mesh) {
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    return indices;
}

// shared by vertex layouts using the (position, normal, texCoo, tangent, bitangent) field names:
// TexturedMeshVertex and SkinnedMeshVertex
template <typename VertexT>
static std::vector<VertexT> extractTexturedVertices(const aiMesh* mesh) {
    std::vector<VertexT> vertices(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        VertexT& vertex = vertices[i];
        vertex.position = math::Vector3f(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->HasNormals()) {
            vertex.normal = math::Vector3f(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        if (mesh->mTextureCoords[0]) {
            vertex.texCoo = math::Vector2f(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        if (mesh->HasTangentsAndBitangents()) {
            vertex.tangent = math::Vector3f(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            vertex.bitangent = math::Vector3f(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        }
    }
    return vertices;
}

// shared by vertex layouts that only carry (position, normal), under whatever field names they use:
// PhongDrawableVertex (pos, nrm) and SkinnedPhongMeshVertex (position, normal)
template <typename VertexT>
static std::vector<VertexT> extractPositionNormalVertices(const aiMesh* mesh, math::Vector3f VertexT::*posField, math::Vector3f VertexT::*nrmField) {
    std::vector<VertexT> vertices(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        vertices[i].*posField = math::Vector3f(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->HasNormals()) {
            vertices[i].*nrmField = math::Vector3f(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
    }
    return vertices;
}

template <typename VertexT>
static void addVertexBoneData(VertexT& vertex, int32_t boneId, float weight) {
    for (int i = 0; i < MAX_VTX_WEIGHTS; i++) {
        if (vertex.weights[i] == 0.0f) {
            vertex.boneIds[i] = boneId;
            vertex.weights[i] = weight;
            return;
        }
    }
}

// assigns each bone a stable index, records which vertices it influences, and writes each
// bone's bind-pose offset into boneOffsets
template <typename VertexT>
static std::unordered_map<std::string, int32_t> buildBoneMapping(const aiMesh* mesh, std::vector<VertexT>& vertices,
        std::unordered_map<std::string, math::Matrix44f>& boneOffsets) {
    std::unordered_map<std::string, int32_t> boneMapping;
    for (unsigned int i = 0; i < mesh->mNumBones; i++) {
        aiBone* bone = mesh->mBones[i];
        std::string boneName(bone->mName.C_Str());

        auto it = boneMapping.find(boneName);
        int32_t boneId;
        if (it == boneMapping.end()) {
            boneId = static_cast<int32_t>(boneMapping.size());
            if (boneId >= MAX_BONES) {
                PANIC("AssimpModel: mesh has more than MAX_BONES (%d) distinct bones", MAX_BONES);
            }
            boneMapping[boneName] = boneId;
        } else {
            boneId = it->second;
        }

        for (unsigned int j = 0; j < bone->mNumWeights; j++) {
            const aiVertexWeight& vw = bone->mWeights[j];
            addVertexBoneData(vertices[vw.mVertexId], boneId, vw.mWeight);
        }

        boneOffsets[boneName] = toMatrix44f(bone->mOffsetMatrix);
    }
    return boneMapping;
}

struct MaterialColors {
    math::Vector3f ambient = math::Vector3f::ZERO;
    math::Vector3f diffuse = math::Vector3f::ZERO;
    math::Vector3f specular = math::Vector3f::ZERO;
    float shininess = 0.0f;
};

static MaterialColors extractMaterialColors(aiMaterial* material) {
    MaterialColors colors;
    aiColor3D color;

    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        colors.diffuse = math::Vector3f(color.r, color.g, color.b);
    }

    if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
        colors.ambient = math::Vector3f(color.r, color.g, color.b);
    } else {
        colors.ambient = colors.diffuse * 0.5f;
    }

    if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
        colors.specular = math::Vector3f(color.r, color.g, color.b);
    }

    material->Get(AI_MATKEY_SHININESS, colors.shininess);

    return colors;
}

AssimpModel::AssimpModel(const char* path) {
    auto pathLength = strlen(path) + 1;
    mPath = (char*)malloc(pathLength * sizeof(char));
    memcpy(mPath, path, pathLength);
}

AssimpModel::~AssimpModel() {
    free(mPath);

    for (auto* mesh : mMeshes) {
        delete mesh;
    }
    for (auto* animation : mAnimations) {
        delete animation;
    }
}

void AssimpModel::load() {
    printf("%s\n", mPath);
    loadModel(mPath);

    for (auto* mesh : mMeshes) {
        addChild(mesh);
    }
}

void AssimpModel::loadModel(const char* path) {
    mScene = mImporter.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights);
    RUNTIME_ASSERT(mScene && (mScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) == 0 && mScene->mRootNode, mImporter.GetErrorString());

    std::string strPath(path);
    mDirectory = strPath.substr(0, strPath.find_last_of('/'));

    processNode(mScene->mRootNode, mScene);
}

void AssimpModel::processNode(aiNode *node, const aiScene *scene) {
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Drawable3d* drawableMesh = processMesh(mesh, scene);
        mMeshes.push_back(drawableMesh);
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Drawable3d* AssimpModel::processMesh(aiMesh *mesh, const aiScene *scene) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // kinda crude way of figuring out how to shade the mesh. Let me know if you know a better way
    bool hasTexture = material->GetTextureCount(aiTextureType_DIFFUSE) > 0 || material->GetTextureCount(aiTextureType_AMBIENT) > 0;
    if (mesh->HasBones()) {
        // skinned mesh
        return hasTexture ? processSkinnedMesh(mesh, scene) : processSkinnedPhongMesh(mesh, scene);
    } else if (hasTexture) {
        // textured mesh
        return processTexturedMesh(mesh, scene);
    } else {
        // phong shaded mesh
        return processPhongMesh(mesh, scene);
    }
}

Drawable3d* AssimpModel::processSkinnedMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<SkinnedMeshVertex> vertices = extractTexturedVertices<SkinnedMeshVertex>(mesh);
    std::vector<unsigned int> indices = extractIndices(mesh);
    std::unordered_map<std::string, int32_t> boneMapping = buildBoneMapping(mesh, vertices, mBoneOffsets);

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<TextureDescriptor> textures = loadStandardMaterialTextures(material);

    SkinnedMesh* skinnedMesh = new SkinnedMesh(vertices, indices, textures, boneMapping);
    mSkinnedMeshes.push_back(skinnedMesh);
    return skinnedMesh;
}

Drawable3d* AssimpModel::processSkinnedPhongMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<SkinnedPhongMeshVertex> vertices = extractPositionNormalVertices<SkinnedPhongMeshVertex>(
        mesh, &SkinnedPhongMeshVertex::position, &SkinnedPhongMeshVertex::normal);
    std::vector<unsigned int> indices = extractIndices(mesh);
    std::unordered_map<std::string, int32_t> boneMapping = buildBoneMapping(mesh, vertices, mBoneOffsets);

    SkinnedPhongMesh* result = new SkinnedPhongMesh(vertices, indices, boneMapping);
    mSkinnedMeshes.push_back(result);

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    MaterialColors colors = extractMaterialColors(material);
    result->setAmbient(colors.ambient);
    result->setDiffuse(colors.diffuse);
    result->setSpecular(colors.specular);
    result->setShininess(colors.shininess);

    return result;
}

Drawable3d* AssimpModel::processPhongMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<PhongDrawableVertex> vertices = extractPositionNormalVertices<PhongDrawableVertex>(
        mesh, &PhongDrawableVertex::pos, &PhongDrawableVertex::nrm);
    std::vector<unsigned int> indices = extractIndices(mesh);

    PhongMesh* res = new PhongMesh(vertices, indices);

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    MaterialColors colors = extractMaterialColors(material);
    res->setAmbient(colors.ambient);
    res->setDiffuse(colors.diffuse);
    res->setSpecular(colors.specular);
    res->setShininess(colors.shininess);

    return res;
}

Drawable3d* AssimpModel::processTexturedMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<TexturedMeshVertex> vertices = extractTexturedVertices<TexturedMeshVertex>(mesh);
    std::vector<unsigned int> indices = extractIndices(mesh);

    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<TextureDescriptor> textures = loadStandardMaterialTextures(material);

    return new TexturedMesh(vertices, indices, textures);
}

std::vector<TextureDescriptor> AssimpModel::loadMaterialTextures(aiMaterial *mat, aiTextureType type, uint32_t binding) {
    std::vector<TextureDescriptor> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        if (i > 0) {
            // ignore textures other than the first
            break;
        }
        aiString str;
        mat->GetTexture(type, i, &str);

        TextureDescriptor texture;
        texture.textureFile = mDirectory + '/' + str.C_Str();
        texture.binding = binding;
        textures.push_back(texture);
    }
    return textures;
}

std::vector<TextureDescriptor> AssimpModel::loadStandardMaterialTextures(aiMaterial *mat) {
    std::vector<TextureDescriptor> textures;
    auto append = [&](aiTextureType type, uint32_t binding) {
        std::vector<TextureDescriptor> maps = loadMaterialTextures(mat, type, binding);
        textures.insert(textures.end(), maps.begin(), maps.end());
    };

    append(aiTextureType_DIFFUSE, 1);
    append(aiTextureType_SPECULAR, 2);
    append(aiTextureType_HEIGHT, 4);
    append(aiTextureType_AMBIENT, 0);
    append(aiTextureType_SHININESS, 3);

    return textures;
}

std::vector<std::string> AssimpModel::getAllAnimationNames() const {
    std::vector<std::string> names;
    names.reserve(mScene->mNumAnimations);
    for (unsigned int i = 0; i < mScene->mNumAnimations; i++) {
        names.push_back(mScene->mAnimations[i]->mName.C_Str());
    }
    return names;
}

AssimpAnimation* AssimpModel::createAnimation(const std::string& name) {
    for (AssimpAnimation* animation : mAnimations) {
        if (animation->name() == name) {
            return animation;
        }
    }

    for (unsigned int i = 0; i < mScene->mNumAnimations; i++) {
        aiAnimation* anim = mScene->mAnimations[i];
        if (name == anim->mName.C_Str()) {
            AssimpAnimation* animation = new AssimpAnimation(*this, anim);
            mAnimations.push_back(animation);
            return animation;
        }
    }

    PANIC("AssimpModel: no animation named \"%s\"", name.c_str());
    return nullptr;
}

const math::Matrix44f* AssimpModel::boneOffset(const std::string& name) const {
    auto it = mBoneOffsets.find(name);
    return it == mBoneOffsets.end() ? nullptr : &it->second;
}

} // gfx
} // bolt
