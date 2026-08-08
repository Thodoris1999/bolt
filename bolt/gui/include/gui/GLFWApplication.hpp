#pragma once

#include "gui/BaseApplication.hpp"
#include "gui/GLFWWindow.hpp"

struct GLFWwindow;

namespace bolt {
namespace gui {

enum class GLFWAppEventType {
    WindowResized,
    Scroll,
    CursorMotion,
    MouseButton,
};

struct GLFWAppEvent {
    GLFWAppEventType type;

    // WindowResized
    int windowWidth = 0;
    int windowHeight = 0;

    // Scroll
    double scrollX = 0;
    double scrollY = 0;

    // CursorMotion
    double cursorX = 0;
    double cursorY = 0;
    double cursorDeltaX = 0;
    double cursorDeltaY = 0;

    // CursorMotion (button state) and MouseButton
    int mouseButton = 0;
    int mouseAction = 0;
    int mouseMods = 0;
};

class GLFWApplication : public BaseApplication {
public:
    GLFWApplication(int width, int height, RenderBackend renderBackend = BACKEND_VULKAN);
    virtual ~GLFWApplication();
    virtual void run() override;
    virtual void handleEvents() override;
    virtual void handleEvent(const GLFWAppEvent&) {}

protected:
    GLFWWindow mWindow;

private:
    void setupOpengl();
    void setupVulkan();

    double mLastCursorX;
    double mLastCursorY;
    bool mHasLastCursor;

    static void windowSizeCallback(GLFWwindow* window, int width, int height);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
};

} // gui
} // bolt
