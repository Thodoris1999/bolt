#include "SDLApplication3d.hpp"

#include "gfx/opengl/OpenglRenderSystem.hpp"
#include "util/common.h"

#include <chrono>
#include <iostream>

#ifdef BOLT_GFX_HAVE_VULKAN
#include "gfx/vulkan/VulkanRenderSystem.hpp"
#include <SDL3/SDL_vulkan.h>
#endif

static void sdlGetFramebufferSize(void* userData, uint32_t& w, uint32_t& h) {
    SDL_Window* window = static_cast<SDL_Window*>(userData);
    int iw, ih;
    SDL_GetWindowSizeInPixels(window, &iw, &ih);
    w = static_cast<uint32_t>(iw);
    h = static_cast<uint32_t>(ih);
}

SDLApplication3d::SDLApplication3d(int width, int height, RenderBackend renderBackend) : SDLApplication(width, height, renderBackend) {
    switch (renderBackend)
    {
    case BACKEND_OPENGL:
        mRenderSystem = new bolt::gfx::OpenglRenderSystem;
        break;
    
#ifdef BOLT_GFX_HAVE_VULKAN
    case BACKEND_VULKAN: {
        bolt::gfx::WindowHooks windowHooks;
        windowHooks.getFramebufferSize = sdlGetFramebufferSize;
        windowHooks.userData = mWindow.getSdlWindow();
        bolt::gfx::VulkanRenderSystem* vlkRenderSystem = new bolt::gfx::VulkanRenderSystem(mVulkanExtensions, mVulkanExtensionCount, windowHooks);
        bool result = SDL_Vulkan_CreateSurface(mWindow.getSdlWindow(), vlkRenderSystem->instance(), nullptr, &vlkRenderSystem->surface());
        RUNTIME_ASSERT(result, SDL_GetError());
        vlkRenderSystem->init();
        mRenderSystem = vlkRenderSystem;
        break;
    }
#endif
    
    default:
        PANIC("Unknown render backend %d", renderBackend);
        break;
    }
    mScene = new bolt::gfx::SceneManager(mRenderSystem);

    mScene->renderSystem()->setViewport(0, 0, width, height);
    mCamera = mScene->createOrbitCamera();
    mScene->root().addChild(mCamera);
}

SDLApplication3d::~SDLApplication3d() {
    delete mRenderSystem;
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
            std::cout << "FPS: " << (frameCount / elapsed.count()) << std::endl;
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
