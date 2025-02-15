#pragma once

#include "SDLWindow.hpp"
#include "gfx/DrawableManager.hpp"

class SDLApplication {
public:
    SDLApplication();
    ~SDLApplication();
    void run();
    void handleEvents();

    bolt::gfx::Drawable3dManager& drawableManager() { return mDrawableManager; }

private:
    SDLWindow mWindow;
    SDL_GLContext mGlContext;
    bolt::gfx::Drawable3dManager mDrawableManager;
    bool mRunning;
};
