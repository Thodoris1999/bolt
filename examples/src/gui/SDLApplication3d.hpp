#pragma once

#include "SDLApplication.hpp"

#include "gfx/SceneManager.hpp"

class SDLApplication3d : public SDLApplication {
public:
    SDLApplication3d(int width, int height);

    virtual void run() override;
    virtual void handleEvent(const SDL_Event& event) override;

    bolt::gfx::SceneManager& scene() { return mScene; }

private:
    bolt::gfx::SceneManager mScene;
    bolt::gfx::OrbitCamera* mCamera;
};
