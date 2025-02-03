#pragma once

#include "SDLWindow.hpp"
#include "gfx/DrawableManager.hpp"

class SDLApplication {
public:
    SDLApplication();
    ~SDLApplication();
    void run();
    void handleEvents();

    Drawable3dManager& drawableManager() { return mDrawableManager; }

private:
    SDLWindow mWindow;
    SDL_GLContext mGlContext;
    Drawable3dManager mDrawableManager;
    bool mRunning;
};
