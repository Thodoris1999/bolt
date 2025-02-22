#include "SDLWindow.hpp"

#include "common.h"

#include "gfx/gl_defines.h"

SDLWindow::SDLWindow() {

}

void SDLWindow::create() {
    mSdlWindow = SDL_CreateWindow(
        "Object Visualizer",
        1000,
        1000,
        SDL_WINDOW_OPENGL
    );

    RUNTIME_ASSERT(mSdlWindow != nullptr, SDL_GetError());
}

void SDLWindow::destroy() {
    SDL_DestroyWindow(mSdlWindow);
}

void SDLWindow::init() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void SDLWindow::beginFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SDLWindow::endFrame() {
    SDL_GL_SwapWindow(mSdlWindow);
}
