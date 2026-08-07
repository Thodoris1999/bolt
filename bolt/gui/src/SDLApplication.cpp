#include "gui/SDLApplication.hpp"
#include "util/common.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "gfx/opengl/gl_defines.h"
#include "gfx/opengl/GlUtils.hpp"
#include "gfx/opengl/OpenglRenderSystem.hpp"
#include "gfx/vulkan/VulkanRenderSystem.hpp"

#include <chrono>

namespace bolt {
namespace gui {

static void sdlGetFramebufferSize(void* userData, uint32_t& w, uint32_t& h) {
    SDL_Window* window = static_cast<SDL_Window*>(userData);
    int iw, ih;
    SDL_GetWindowSizeInPixels(window, &iw, &ih);
    w = static_cast<uint32_t>(iw);
    h = static_cast<uint32_t>(ih);
}

SDLApplication::SDLApplication(int initWidth, int initHeight, RenderBackend renderBackend) : mRenderBackend(renderBackend), mRunning(true) {
    // Initialize SDL
#ifndef NDEBUG
    // SDL lets DBus leak by default in case other systems use it. Disable this to not confuse ASan
    bool hintSet = SDL_SetHint(SDL_HINT_SHUTDOWN_DBUS_ON_QUIT, "1");
    RUNTIME_ASSERT(hintSet, SDL_GetError());
#endif
    bool videoInit = SDL_InitSubSystem(SDL_INIT_VIDEO);
    RUNTIME_ASSERT(videoInit, SDL_GetError());

    if (renderBackend == BACKEND_OPENGL) {
        // Set OpenGL context attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifndef NDEBUG
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
    }

    SDLWindowOptions options;
    options.renderBackend = renderBackend;
    mWindow.create(initWidth, initHeight, options);

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

void SDLApplication::setupOpengl() {
    // Create OpenGL context
    mGlContext = SDL_GL_CreateContext(mWindow.getSdlWindow());
    RUNTIME_ASSERT(mGlContext, SDL_GetError());

    // Initialize GLAD
    auto gladLoader = gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
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

void SDLApplication::setupVulkan() {
    unsigned int extensionCount;
    const char* const * extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    RUNTIME_ASSERT(extensions, SDL_GetError());

    bolt::gfx::WindowHooks windowHooks;
    windowHooks.getFramebufferSize = sdlGetFramebufferSize;
    windowHooks.userData = mWindow.getSdlWindow();
    bolt::gfx::VulkanRenderSystem* vlkRenderSystem = new bolt::gfx::VulkanRenderSystem(extensions, extensionCount, windowHooks);
    bool result = SDL_Vulkan_CreateSurface(mWindow.getSdlWindow(), vlkRenderSystem->instance(), nullptr, &vlkRenderSystem->surface());
    RUNTIME_ASSERT(result, SDL_GetError());
    vlkRenderSystem->init();
    mRenderSystem = vlkRenderSystem;
}

SDLApplication::~SDLApplication() {
    delete mRenderSystem;
    SDL_GL_DestroyContext(mGlContext);
    mWindow.destroy();
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}

void SDLApplication::run() {
    while (mRunning) {
        handleEvents();
        // do more interesting stuff in subclasses like physics, rendering and windsurfing
        mWindow.swapBuffers();
    }
}

void SDLApplication::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            mRunning = false;
            break;
        default:
            handleEvent(event);
        }
    }
}

} // gui
} // bolt
