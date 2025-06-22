#include "SDLWindow.hpp"

#include "util/common.h"

#include "gfx/gl_defines.h"

SDLWindow::SDLWindow() {

}

void SDLWindow::create(int width, int height) {
    mWidth = width;
    mHeight = height;
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

void SDLWindow::init() {
    glViewport(0, 0, mWidth, mHeight);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void SDLWindow::onResize(int width, int height) {
    glViewport(0, 0, width, height);
}

void SDLWindow::beginFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SDLWindow::endFrame() {
    SDL_GL_SwapWindow(mSdlWindow);
}
