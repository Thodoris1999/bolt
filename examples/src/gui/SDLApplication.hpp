#pragma once

#include "SDLWindow.hpp"

#include <functional>

class SDLApplication {
public:
    SDLApplication(int width, int height, RenderBackend renderBackend = BACKEND_VULKAN);
    virtual ~SDLApplication();
    virtual void run();
    void handleEvents();
    virtual void handleEvent(const SDL_Event&) {}

protected:
    SDLWindow mWindow;
    RenderBackend mRenderBackend;
    bool mRunning;

    SDL_GLContext mGlContext;
    unsigned int mVulkanExtensionCount;
    const char* const * mVulkanExtensions;

private:
    void setupOpengl();
    void setupVulkan();
};
