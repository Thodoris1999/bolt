#include "SDLWindow.hpp"

#include "util/common.h"

SDLWindow::SDLWindow() {

}

void SDLWindow::create(int width, int height) {
    mSdlWindow = SDL_CreateWindow(
        "Object Visualizer",
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    RUNTIME_ASSERT(mSdlWindow != nullptr, SDL_GetError());
}

void SDLWindow::destroy() {
    SDL_DestroyWindow(mSdlWindow);
}

void SDLWindow::swapBuffers() {
    SDL_GL_SwapWindow(mSdlWindow);
}
