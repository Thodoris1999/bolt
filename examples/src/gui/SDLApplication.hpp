#pragma once

#include "SDLWindow.hpp"

#include <functional>

class SDLApplication {
public:
    SDLApplication(int width, int height);
    virtual ~SDLApplication();
    virtual void run();
    void handleEvents();
    virtual void handleEvent(const SDL_Event&) {}

protected:
    SDLWindow mWindow;
    SDL_GLContext mGlContext;
    bool mRunning;
};
