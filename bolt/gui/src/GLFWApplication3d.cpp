#include "gui/GLFWApplication3d.hpp"

#include <GLFW/glfw3.h>

namespace bolt {
namespace gui {

GLFWApplication3d::GLFWApplication3d(int width, int height, RenderBackend renderBackend) : GLFWApplication(width, height, renderBackend) {
    mScene = new bolt::gfx::SceneManager(mRenderSystem);
    mCamera = mScene->createOrbitCamera();
    mScene->root().addChild(mCamera);
}

GLFWApplication3d::~GLFWApplication3d() {
    delete mScene;
}

void GLFWApplication3d::run() {
    while (mRunning) {
        handleEvents();

        update();

        mScene->draw();
        mWindow.swapBuffers();
    }
}

void GLFWApplication3d::handleEvent(const GLFWAppEvent& event) {
    switch (event.type) {
    case GLFWAppEventType::WindowResized:
        mScene->renderSystem()->setViewport(0, 0, event.windowWidth, event.windowHeight);
        mCamera->setAspectRatio(event.windowWidth / (float)event.windowHeight);
        break;
    case GLFWAppEventType::Scroll:
        // zoom
        mCamera->onScroll(0.1 * event.scrollY);
        break;
    case GLFWAppEventType::CursorMotion:
        // drag
        if (event.mouseButton == GLFW_MOUSE_BUTTON_MIDDLE && event.mouseAction == GLFW_PRESS) {
            mCamera->onDrag(event.cursorDeltaX, event.cursorDeltaY);
        }
        break;
    default:
        break;
    }
}

} // gui
} // bolt
