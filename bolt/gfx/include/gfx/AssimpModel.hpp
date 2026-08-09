#pragma once

#include "Drawable.hpp"

#include "math/Matrix.hpp"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include <vector>
#include <string>
#include <unordered_map>

namespace bolt {
namespace gfx {

class SkinnedMesh;
class AssimpAnimation;

class AssimpModel : public SceneNode {
public:
    AssimpModel(const char* path);
    ~AssimpModel();

    void load();

    std::vector<Drawable3d*>& meshes() { return mMeshes; }
    const std::vector<SkinnedMesh*>& skinnedMeshes() const { return mSkinnedMeshes; }

    const aiScene* scene() const { return mScene; }

    std::vector<std::string> getAllAnimationNames() const;
    /// Looks up (or creates, on first call for a given name) the AssimpAnimation for the animation
    /// named \p name. Owned by this AssimpModel; PANICs if no such animation exists.
    AssimpAnimation* createAnimation(const std::string& name);

    /// Bind-pose (inverse) transform of the bone named \p name, or nullptr if this model has no such bone
    const math::Matrix44f* boneOffset(const std::string& name) const;

private:
    void loadModel(const char* path);
    void processNode(aiNode *node, const aiScene *scene);
    Drawable3d* processMesh(aiMesh *mesh, const aiScene *scene);
    Drawable3d* processSkinnedMesh(aiMesh *mesh, const aiScene *scene);
    Drawable3d* processTexturedMesh(aiMesh *mesh, const aiScene *scene);
    Drawable3d* processPhongMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<TextureDescriptor> loadMaterialTextures(aiMaterial *mat, aiTextureType type, uint32_t binding);

    Assimp::Importer mImporter;
    const aiScene* mScene = nullptr;
    std::vector<Drawable3d*> mMeshes;
    std::vector<SkinnedMesh*> mSkinnedMeshes;
    std::unordered_map<std::string, math::Matrix44f> mBoneOffsets;
    std::vector<AssimpAnimation*> mAnimations;
    std::string mDirectory;
    char* mPath;
};

} // gfx
} // bolt
