#pragma once

#include <SDL3/SDL.h>

class SDLWindow {
public:
    SDLWindow();
    /// Creates the window using the underlying platform's windowing system
    void create(int width, int height);
    /// Destroys the window using the underlying platform's windowing system
    void destroy();
    /// Switches in rendered frame buffer to be presented
    void swapBuffers();

    SDL_Window* getSdlWindow() const { return mSdlWindow; }

private:
    SDL_Window* mSdlWindow;
};
