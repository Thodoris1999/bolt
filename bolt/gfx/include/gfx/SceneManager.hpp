#pragma once

#include "gfx/Camera.hpp"
#include "gfx/Drawable.hpp"
#include "gfx/DirLight.hpp"
#include "gfx/SceneNode.hpp"
#include "gfx/RenderSystem.hpp"

#include <vector>

namespace bolt {
namespace gfx {

class SceneManager {
public:
    SceneManager(RenderSystem* renderSystem);
    ~SceneManager();

    /// @beginGetters
    SceneNode& root() { return mSceneRoot; }
    RenderSystem* renderSystem() { return mRenderSystem; }
    /// @endGetters

    template <typename T, typename... Args>
    T* createDrawable(Args&&... args) {
        static_assert(std::is_base_of<Drawable3d, T>::value, "drawable must derive from Drawable3d");
        T* drawable = new T(std::forward<Args>(args)...);
        mDrawables.push_back(drawable);
        return drawable;
    }
    OrbitCamera* createOrbitCamera();

    /// Detaches drawable from wherever it's parented in the scene graph, frees its backend GPU
    /// resources, and destroys it. Safe to call at any time, including after loadAll().
    void removeDrawable(Drawable3d* drawable);

    /// Registers and loads every drawable reachable from root() that isn't loaded yet. Safe to
    /// call more than once, e.g. after createDrawable() + addChild() add something new to a scene
    /// that's already rendering.
    void loadAll();
    /// Performs draw calls for all drawables
    void draw(Camera* camera);
    void draw();

private:
    void addDrawableRecurse(SceneNode* node);

    SceneNode mSceneRoot;
    RenderSystem* mRenderSystem;
    std::vector<Drawable3d*> mDrawables;
    std::vector<Camera*> mCameras;
    std::vector<RenderUniformBuffer*> mUniforms;
    DirLightParams mDirLightParams;
    bool mLoaded = false;
};

} // gfx
} // bolt
