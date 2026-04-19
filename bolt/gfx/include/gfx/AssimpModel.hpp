#pragma once

#include "Drawable.hpp"

#include <assimp/scene.h>

#include <vector>
#include <string>

namespace bolt {
namespace gfx {

class AssimpModel : public SceneNode {
public:
    AssimpModel(const char* path);
    ~AssimpModel();
    
    void load();

    std::vector<Drawable3d*>& meshes() { return mMeshes; }

private:
    void loadModel(const char* path);
    void processNode(aiNode *node, const aiScene *scene);
    Drawable3d* processMesh(aiMesh *mesh, const aiScene *scene);
    Drawable3d* processTexturedMesh(aiMesh *mesh, const aiScene *scene);
    Drawable3d* processPhongMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<TextureDescriptor> loadMaterialTextures(aiMaterial *mat, aiTextureType type, uint32_t binding);

    std::vector<Drawable3d*> mMeshes;
    std::string mDirectory;
    char* mPath;
};

} // gfx
} // bolt
