#pragma once

#include "SDLApplication.hpp"

#include "gfx/SceneManager.hpp"
#include "gui/SDLWindow.hpp"

class SDLApplication3d : public SDLApplication {
public:
    SDLApplication3d(int width, int height, RenderBackend renderBackend = BACKEND_VULKAN);
    virtual ~SDLApplication3d();

    virtual void run() override;
    virtual void handleEvent(const SDL_Event& event) override;
    virtual void update() {}

    bolt::gfx::SceneManager* scene() { return mScene; }

protected:
    bolt::gfx::SceneManager* mScene;
    bolt::gfx::OrbitCamera* mCamera;
};
