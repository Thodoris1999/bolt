#include "SDLApplication3d.hpp"

#include <chrono>
#include <iostream>

SDLApplication3d::SDLApplication3d(int width, int height, RenderBackend renderBackend) : SDLApplication(width, height, renderBackend) {
    mScene = new bolt::gfx::SceneManager(mRenderSystem);
    mCamera = mScene->createOrbitCamera();
    mScene->root().addChild(mCamera);
}

SDLApplication3d::~SDLApplication3d() {
    delete mScene;
}

void SDLApplication3d::run() {
    uint32_t frameCount = 0;
    auto fpsWindowStart = std::chrono::steady_clock::now();

    while (mRunning) {
        handleEvents();

        update();

        mScene->draw();
        mWindow.swapBuffers();

        frameCount++;
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - fpsWindowStart);
        if (elapsed.count() >= 1.0) {
            std::cout << "FPS: " << (frameCount / elapsed.count()) << '\n';
            frameCount = 0;
            fpsWindowStart = now;
        }
    }
}

void SDLApplication3d::handleEvent(const SDL_Event& event) {
    switch (event.type) {
    case SDL_EVENT_WINDOW_RESIZED:
        mScene->renderSystem()->setViewport(0, 0, event.window.data1, event.window.data2);
        mCamera->setAspectRatio(event.window.data1 / (float)event.window.data2);
        break;
    case SDL_EVENT_MOUSE_WHEEL:
        // zoom
        mCamera->onScroll(0.1 * event.wheel.y);
        break;
    case SDL_EVENT_MOUSE_MOTION:
        // drag
        if ((event.motion.state & SDL_BUTTON_MIDDLE) != 0) {
            mCamera->onDrag(event.motion.xrel, event.motion.yrel);
        }
        break;
    }
}
