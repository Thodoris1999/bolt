#pragma once

#include "gui/BaseApplication.hpp"
#include "gui/SDLWindow.hpp"

namespace bolt {
namespace gui {

class SDLApplication : public BaseApplication {
public:
    SDLApplication(int width, int height, RenderBackend renderBackend = BACKEND_VULKAN);
    virtual ~SDLApplication();
    virtual void run() override;
    virtual void handleEvents() override;
    virtual void handleEvent(const SDL_Event&) {}

protected:
    SDLWindow mWindow;
    SDL_GLContext mGlContext;

private:
    void setupOpengl();
    void setupVulkan();
};

} // gui
} // bolt
