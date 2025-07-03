#pragma once

#include "SDLWindow.hpp"

#include <functional>

class SDLApplication {
public:
    using EventHandler = std::function<void(const SDL_Event&)>;
    using Runner = std::function<void()>;

    SDLApplication(int width, int height);
    ~SDLApplication();
    void run();
    void handleEvents();

    void setEventHandler(EventHandler handler) { mEventHandler = handler; }
    void setRunner(Runner handler) { mRunner = handler; }

private:
    SDLWindow mWindow;
    SDL_GLContext mGlContext;
    EventHandler mEventHandler;
    Runner mRunner;
    bool mRunning;
};
