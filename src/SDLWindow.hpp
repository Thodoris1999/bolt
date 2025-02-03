#pragma once

#include <SDL3/SDL.h>

class SDLWindow {
public:
    SDLWindow();
    void create();
    void destroy();
    void init();
    void beginFrame();
    void endFrame();

    SDL_Window* getSdlWindow() const { return mSdlWindow; }

private:
    SDL_Window* mSdlWindow;
};
