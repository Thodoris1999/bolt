#include "gui/GLFWWindow.hpp"

#include "util/common.h"

#include <GLFW/glfw3.h>

namespace bolt {
namespace gui {

const GLFWWindowOptions defaultGLFWWindowOptions {
    BACKEND_VULKAN
};

void GLFWWindow::create(int width, int height, const GLFWWindowOptions& options) {
    mRenderBackend = options.renderBackend;
    switch (options.renderBackend)
    {
    case BACKEND_OPENGL:
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef NDEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
        break;

    case BACKEND_VULKAN:
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        break;

    default:
        PANIC("Unknown render backend %d", options.renderBackend);
        break;
    }
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    mGlfwWindow = glfwCreateWindow(width, height, "Object Visualizer", nullptr, nullptr);
    RUNTIME_ASSERT(mGlfwWindow != nullptr, "Failed to create GLFW window");
}

void GLFWWindow::destroy() {
    glfwDestroyWindow(mGlfwWindow);
}

void GLFWWindow::swapBuffers() {
    if (mRenderBackend == BACKEND_OPENGL) {
        glfwSwapBuffers(mGlfwWindow);
    }
}

} // gui
} // bolt
