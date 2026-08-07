#include "SDLWindow.hpp"

#include "util/common.h"

const SDLWindowOptions defaultWindowOptions {
    BACKEND_VULKAN
};

void SDLWindow::create(int width, int height, const SDLWindowOptions& options) {
    SDL_WindowFlags windowFlags = SDL_WINDOW_RESIZABLE;
    switch (options.renderBackend)
    {
    case BACKEND_OPENGL:
        windowFlags |= SDL_WINDOW_OPENGL;
        break;

    case BACKEND_VULKAN:
        windowFlags |= SDL_WINDOW_VULKAN;
        break;
    
    default:
        PANIC("Unknown render backend %d", options.renderBackend);
        break;
    }
    mSdlWindow = SDL_CreateWindow(
        "Object Visualizer",
        width,
        height,
        windowFlags
    );

    RUNTIME_ASSERT(mSdlWindow != nullptr, SDL_GetError());
}

void SDLWindow::destroy() {
    SDL_DestroyWindow(mSdlWindow);
}

void SDLWindow::swapBuffers() {
    SDL_GL_SwapWindow(mSdlWindow);
}
