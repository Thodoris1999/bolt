#include "gui/GLFWApplication.hpp"
#include "util/common.h"

#include "gfx/opengl/gl_defines.h"
#include "gfx/opengl/GlUtils.hpp"
#include "gfx/opengl/OpenglRenderSystem.hpp"
#include "gfx/vulkan/VulkanRenderSystem.hpp"

#include <GLFW/glfw3.h>

namespace bolt {
namespace gui {

static void glfwHookGetFramebufferSize(void* userData, uint32_t& w, uint32_t& h) {
    GLFWwindow* window = static_cast<GLFWwindow*>(userData);
    int iw, ih;
    glfwGetFramebufferSize(window, &iw, &ih);
    w = static_cast<uint32_t>(iw);
    h = static_cast<uint32_t>(ih);
}

static void glfwErrorCallback(int error, const char* description) {
    ERROR("GLFW error %d: %s", error, description);
}

GLFWApplication::GLFWApplication(int initWidth, int initHeight, RenderBackend renderBackend)
    : BaseApplication(renderBackend), mLastCursorX(0), mLastCursorY(0), mHasLastCursor(false) {
    glfwSetErrorCallback(glfwErrorCallback);
    bool initialized = glfwInit();
    RUNTIME_ASSERT(initialized, "Failed to initialize GLFW");

    GLFWWindowOptions options;
    options.renderBackend = renderBackend;
    mWindow.create(initWidth, initHeight, options);

    GLFWwindow* window = mWindow.getGlfwWindow();
    glfwSetWindowUserPointer(window, this);
    glfwSetWindowSizeCallback(window, &GLFWApplication::windowSizeCallback);
    glfwSetScrollCallback(window, &GLFWApplication::scrollCallback);
    glfwSetCursorPosCallback(window, &GLFWApplication::cursorPosCallback);
    glfwSetMouseButtonCallback(window, &GLFWApplication::mouseButtonCallback);

    switch (renderBackend) {
    case BACKEND_OPENGL:
        setupOpengl();
        break;

    case BACKEND_VULKAN:
        setupVulkan();
        break;

    default:
        PANIC("Unknown render backend %d", renderBackend);
        break;
    }

    mRenderSystem->setViewport(0, 0, initWidth, initHeight);
}

void GLFWApplication::setupOpengl() {
    glfwMakeContextCurrent(mWindow.getGlfwWindow());

    // Initialize GLAD
    auto gladLoader = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    RUNTIME_ASSERT(gladLoader != 0, "Failed to initialize GLAD");

    DEBUG("OpenGL Version: %s", glGetString(GL_VERSION));
#ifndef NDEBUG
    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        // initialize debug output
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(bolt::gfx::openglDebugOutputCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    } else {
        printf("Expected opengl debug enabled!\n");
        std::abort();
    }
#endif

    mRenderSystem = new bolt::gfx::OpenglRenderSystem;
}

void GLFWApplication::setupVulkan() {
    uint32_t extensionCount;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    RUNTIME_ASSERT(extensions, "Failed to query required Vulkan instance extensions");

    bolt::gfx::WindowHooks windowHooks;
    windowHooks.getFramebufferSize = glfwHookGetFramebufferSize;
    windowHooks.userData = mWindow.getGlfwWindow();
    bolt::gfx::VulkanRenderSystem* vlkRenderSystem = new bolt::gfx::VulkanRenderSystem(extensions, extensionCount, windowHooks);
    VkResult result = glfwCreateWindowSurface(vlkRenderSystem->instance(), mWindow.getGlfwWindow(), nullptr, &vlkRenderSystem->surface());
    RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan surface");
    vlkRenderSystem->init();
    mRenderSystem = vlkRenderSystem;
}

GLFWApplication::~GLFWApplication() {
    delete mRenderSystem;
    mWindow.destroy();
    glfwTerminate();
}

void GLFWApplication::run() {
    while (mRunning) {
        handleEvents();
        // do more interesting stuff in subclasses like physics, rendering and windsurfing
        mWindow.swapBuffers();
    }
}

void GLFWApplication::handleEvents() {
    glfwPollEvents();
    if (glfwWindowShouldClose(mWindow.getGlfwWindow())) {
        mRunning = false;
    }
}

void GLFWApplication::windowSizeCallback(GLFWwindow* window, int width, int height) {
    auto* self = static_cast<GLFWApplication*>(glfwGetWindowUserPointer(window));
    GLFWAppEvent event;
    event.type = GLFWAppEventType::WindowResized;
    event.windowWidth = width;
    event.windowHeight = height;
    self->handleEvent(event);
}

void GLFWApplication::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto* self = static_cast<GLFWApplication*>(glfwGetWindowUserPointer(window));
    GLFWAppEvent event;
    event.type = GLFWAppEventType::Scroll;
    event.scrollX = xoffset;
    event.scrollY = yoffset;
    self->handleEvent(event);
}

void GLFWApplication::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* self = static_cast<GLFWApplication*>(glfwGetWindowUserPointer(window));

    GLFWAppEvent event;
    event.type = GLFWAppEventType::CursorMotion;
    event.cursorX = xpos;
    event.cursorY = ypos;
    event.cursorDeltaX = self->mHasLastCursor ? xpos - self->mLastCursorX : 0.0;
    event.cursorDeltaY = self->mHasLastCursor ? ypos - self->mLastCursorY : 0.0;
    event.mouseButton = GLFW_MOUSE_BUTTON_MIDDLE;
    event.mouseAction = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);

    self->mLastCursorX = xpos;
    self->mLastCursorY = ypos;
    self->mHasLastCursor = true;

    self->handleEvent(event);
}

void GLFWApplication::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto* self = static_cast<GLFWApplication*>(glfwGetWindowUserPointer(window));
    GLFWAppEvent event;
    event.type = GLFWAppEventType::MouseButton;
    event.mouseButton = button;
    event.mouseAction = action;
    event.mouseMods = mods;
    self->handleEvent(event);
}

} // gui
} // bolt
