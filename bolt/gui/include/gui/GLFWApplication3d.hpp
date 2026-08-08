#pragma once

#include "gui/GLFWApplication.hpp"

#include "gfx/SceneManager.hpp"

namespace bolt {
namespace gui {

class GLFWApplication3d : public GLFWApplication {
public:
    GLFWApplication3d(int width, int height, RenderBackend renderBackend = BACKEND_VULKAN);
    virtual ~GLFWApplication3d();

    virtual void run() override;
    virtual void handleEvent(const GLFWAppEvent& event) override;
    virtual void update() {}

    bolt::gfx::SceneManager* scene() { return mScene; }

protected:
    bolt::gfx::SceneManager* mScene;
    bolt::gfx::OrbitCamera* mCamera;
};

} // gui
} // bolt
