#pragma once

#include "gfx/Camera.hpp"
#include "gfx/Drawable.hpp"
#include "gfx/SceneNode.hpp"

#include <vector>

namespace bolt {
namespace gfx {

class SceneManager {
public:
    SceneManager();
    ~SceneManager();

    /// @beginGetters
    SceneNode& root() { return mSceneRoot; }
    /// @endGetters

    template <typename T, typename... Args>
    T* createDrawable(Args&&... args) {
        static_assert(std::is_base_of<Drawable3d, T>::value, "drawable must derive from Drawable3d");
        T* drawable = new T(std::forward<Args>(args)...);
        mDrawables.push_back(drawable);
        return drawable;
    }
    OrbitCamera* createOrbitCamera();

    /// Convenience call that calls load for add drawables and cameras
    void loadAll();
    /// Performs draw calls for all drawables
    void draw(Camera* camera);
    void draw();

private:
    SceneNode mSceneRoot;
    std::vector<Drawable3d*> mDrawables;
    std::vector<Camera*> mCameras;
};

} // gfx
} // bolt
