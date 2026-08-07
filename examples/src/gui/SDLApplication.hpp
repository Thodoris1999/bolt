#pragma once

#include "SDLWindow.hpp"

#include <functional>

namespace bolt { namespace gfx { class RenderSystem; } }

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
    bolt::gfx::RenderSystem* mRenderSystem;

private:
    void setupOpengl();
    void setupVulkan();
};
