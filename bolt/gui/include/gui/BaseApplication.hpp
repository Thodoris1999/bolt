#pragma once

#include "gui/RenderBackend.hpp"

namespace bolt { namespace gfx { class RenderSystem; } }

namespace bolt {
namespace gui {

class BaseApplication {
public:
    explicit BaseApplication(RenderBackend renderBackend) : mRenderBackend(renderBackend), mRunning(true), mRenderSystem(nullptr) {}
    virtual ~BaseApplication() {}

    virtual void run() = 0;
    virtual void handleEvents() = 0;

protected:
    RenderBackend mRenderBackend;
    bool mRunning;
    bolt::gfx::RenderSystem* mRenderSystem;
};

} // gui
} // bolt
