#include "SDLApplication3d.hpp"

#include <chrono>

SDLApplication3d::SDLApplication3d(int width, int height) : SDLApplication(width, height) {
    mScene.renderSystem()->setViewport(0, 0, width, height);
    mCamera = mScene.createOrbitCamera();
    mScene.root().addChild(mCamera);
}

void SDLApplication3d::run() {
    while (mRunning) {
        auto start = std::chrono::steady_clock::now();
        handleEvents();

        mScene.draw();
        mWindow.swapBuffers();
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        // Print the elapsed time
        //std::cout << "Elapsed time: " << duration.count() << " microseconds" << std::endl;
    }
}

void SDLApplication3d::handleEvent(const SDL_Event& event) {
    switch (event.type) {
    case SDL_EVENT_WINDOW_RESIZED:
        mScene.renderSystem()->setViewport(0, 0, event.window.data1, event.window.data2);
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
