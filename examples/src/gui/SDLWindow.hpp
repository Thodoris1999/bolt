#pragma once

#include <SDL3/SDL.h>

enum RenderBackend {
    BACKEND_OPENGL,
    BACKEND_VULKAN,
};

struct SDLWindowOptions {
    RenderBackend renderBackend;
};

extern const SDLWindowOptions defaultWindowOptions;

class SDLWindow {
public:
    /// Creates the window using the underlying platform's windowing system
    void create(int width, int height, const SDLWindowOptions& options = defaultWindowOptions);
    /// Destroys the window using the underlying platform's windowing system
    void destroy();
    /// Switches in rendered frame buffer to be presented
    void swapBuffers();

    SDL_Window* getSdlWindow() const { return mSdlWindow; }

private:
    SDL_Window* mSdlWindow;
};
