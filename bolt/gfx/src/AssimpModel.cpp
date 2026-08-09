#include "gfx/AssimpModel.hpp"
#include "gfx/AssimpAnimation.hpp"
#include "gfx/TexturedMesh.hpp"
#include "gfx/PhongMesh.hpp"
#include "gfx/SkinnedMesh.hpp"

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
    if (mesh->HasBones()) {
        // skinned mesh
        return processSkinnedMesh(mesh, scene);
    } else if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0 || material->GetTextureCount(aiTextureType_AMBIENT) > 0) {
        // textured mesh
        return processTexturedMesh(mesh, scene);
    } else {
        // phong shaded mesh
        return processPhongMesh(mesh, scene);
    }
}

static void addVertexBoneData(SkinnedMeshVertex& vertex, int32_t boneId, float weight) {
    for (int i = 0; i < MAX_VTX_WEIGHTS; i++) {
        if (vertex.weights[i] == 0.0f) {
            vertex.boneIds[i] = boneId;
            vertex.weights[i] = weight;
            return;
        }
    }
}

Drawable3d* AssimpModel::processSkinnedMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<SkinnedMeshVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureDescriptor> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        SkinnedMeshVertex vertex;
        math::Vector3f vector;
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // texture coordinates
        if(mesh->mTextureCoords[0])
        {
            math::Vector2f vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoo = vec;
        }
        else
            vertex.texCoo = math::Vector2f(0.0f, 0.0f);

        // tangent
        if (mesh->HasTangentsAndBitangents()) {
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        }

        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // bone weights: assign each bone a stable index and record which vertices it influences
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

        mBoneOffsets[boneName] = toMatrix44f(bone->mOffsetMatrix);
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<TextureDescriptor> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, 1);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<TextureDescriptor> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, 2);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<TextureDescriptor> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, 4);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    std::vector<TextureDescriptor> ambientMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, 0);
    textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());
    std::vector<TextureDescriptor> shininessMaps = loadMaterialTextures(material, aiTextureType_SHININESS, 3);
    textures.insert(textures.end(), shininessMaps.begin(), shininessMaps.end());

    SkinnedMesh* skinnedMesh = new SkinnedMesh(vertices, indices, textures, boneMapping);
    mSkinnedMeshes.push_back(skinnedMesh);
    return skinnedMesh;
}

Drawable3d* AssimpModel::processPhongMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<PhongDrawableVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureDescriptor> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        PhongDrawableVertex vertex;
        math::Vector3f vector;
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.pos = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.nrm = vector;
        }

        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);        
    }

    PhongMesh* res = new PhongMesh(vertices, indices);

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
    aiColor3D color;
    float shininess;

    // DIFFUSE COLOR
    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
        res->setDiffuse(math::Vector3f(color.r, color.g, color.b));
    }

    // AMBIENT COLOR
    if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
        res->setAmbient(math::Vector3f(color.r, color.g, color.b));
    } else {
        res->setAmbient(res->diffuse() * 0.5f);
    }

    // SPECULAR COLOR
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
        res->setSpecular(math::Vector3f(color.r, color.g, color.b));
    }

    // SHININESS
    if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
        res->setShininess(shininess);
    }

    return res;
}

Drawable3d* AssimpModel::processTexturedMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<TexturedMeshVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureDescriptor> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        TexturedMeshVertex vertex;
        math::Vector3f vector;
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // texture coordinates
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            math::Vector2f vec;
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoo = vec;
        }
        else
            vertex.texCoo = math::Vector2f(0.0f, 0.0f);

        // tangent
        if (mesh->HasTangentsAndBitangents()) {
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        }

        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);        
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    std::vector<TextureDescriptor> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, 1);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<TextureDescriptor> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, 2);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<TextureDescriptor> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, 4);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. ambient maps
    std::vector<TextureDescriptor> ambientMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, 0);
    textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());
    // 5. Shininess maps
    std::vector<TextureDescriptor> shininessMaps = loadMaterialTextures(material, aiTextureType_SHININESS, 3);
    textures.insert(textures.end(), shininessMaps.begin(), shininessMaps.end());
    
    // return a mesh object created from the extracted mesh data
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
