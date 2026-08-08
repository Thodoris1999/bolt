#pragma once

#include "gui/RenderBackend.hpp"

struct GLFWwindow;

namespace bolt {
namespace gui {

struct GLFWWindowOptions {
    RenderBackend renderBackend;
};

extern const GLFWWindowOptions defaultGLFWWindowOptions;

class GLFWWindow {
public:
    /// Creates the window using the underlying platform's windowing system
    void create(int width, int height, const GLFWWindowOptions& options = defaultGLFWWindowOptions);
    /// Destroys the window using the underlying platform's windowing system
    void destroy();
    /// Switches in rendered frame buffer to be presented
    void swapBuffers();

    GLFWwindow* getGlfwWindow() const { return mGlfwWindow; }

private:
    GLFWwindow* mGlfwWindow;
    RenderBackend mRenderBackend;
};

} // gui
} // bolt
