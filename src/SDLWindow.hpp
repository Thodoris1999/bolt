#pragma once

#include <SDL3/SDL.h>

class SDLWindow {
public:
    SDLWindow();
    /// Creates the window using the underlying platform's windowing system
    void create(int width, int height);
    /// Destroys the window using the underlying platform's windowing system
    void destroy();
    /// Performs any neccessary graphics initialization for subsequent rendering. Required to be called once before beingFrame() is called
    void init();
    void beginFrame();
    void endFrame();

    SDL_Window* getSdlWindow() const { return mSdlWindow; }
    void onResize(int width, int height);

private:
    int mWidth;
    int mHeight;
    SDL_Window* mSdlWindow;
};
