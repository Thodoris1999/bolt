#include "SDLApplication.hpp"
#include "common.h"

#include <SDL3/SDL.h>

#include "gfx/gl_defines.h"
#include "gfx/GlUtils.hpp"

#include <chrono>

SDLApplication::SDLApplication() : mRunning(true) {
    // Initialize SDL
    bool videoInit = SDL_InitSubSystem(SDL_INIT_VIDEO);
    RUNTIME_ASSERT(videoInit, SDL_GetError());

    // Set OpenGL context attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifndef NDEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    mWindow.create();

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
        printf("Expected opengl debug engabled!\n");
        std::abort();
    }
#endif

    mWindow.init();
    mDrawableManager.init();
}

SDLApplication::~SDLApplication() {
    SDL_GL_DestroyContext(mGlContext);
    mWindow.destroy();
    SDL_Quit();
}

void SDLApplication::run() {
    while (mRunning) {
        auto start = std::chrono::steady_clock::now();
        handleEvents();

        mWindow.beginFrame();
        mDrawableManager.draw();
        mWindow.endFrame();
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        // Print the elapsed time
        //std::cout << "Elapsed time: " << duration.count() << " microseconds" << std::endl;
    }
}

void SDLApplication::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            mRunning = false;
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            // zoom
            mDrawableManager.camera()->onScroll(event.wheel.y);
            break;
        case SDL_EVENT_MOUSE_MOTION:
            // drag
            if ((event.motion.state & SDL_BUTTON_MIDDLE) != 0) {
                mDrawableManager.camera()->onDrag(event.motion.xrel, event.motion.yrel);
            }
            break;
        }
    }
}
