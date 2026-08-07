#include "ImguiApplication.hpp"

#include "gui/SDLWindow.hpp"
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"

/**
 * Largely inspired by imgui examples
 */

ImguiApplication::ImguiApplication(int width, int height) : SDLApplication3d(width, height, BACKEND_OPENGL) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup scaling
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    const char* glsl_version = "#version 330";
    ImGui_ImplSDL3_InitForOpenGL(mWindow.getSdlWindow(), mGlContext);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

ImguiApplication::~ImguiApplication() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void ImguiApplication::run() {
    while (mRunning) {
        handleEvents();

        update();

        // bolt draw
        mScene->draw();

        // imgui draw
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        drawImgui();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // swap buffers
        mWindow.swapBuffers();
    }
}

void ImguiApplication::handleEvent(const SDL_Event& event) {
    ImGui_ImplSDL3_ProcessEvent(&event);
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
