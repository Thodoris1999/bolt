#pragma once

#if defined(BOLT_GUI_HAVE_GLFW)

#include "gui/GLFWApplication.hpp"
#include "gui/GLFWApplication3d.hpp"

namespace bolt {
namespace gui {
using Application = GLFWApplication;
using Application3d = GLFWApplication3d;
} // gui
} // bolt

#elif defined(BOLT_GUI_HAVE_SDL3)

#include "gui/SDLApplication.hpp"
#include "gui/SDLApplication3d.hpp"

namespace bolt {
namespace gui {
using Application = SDLApplication;
using Application3d = SDLApplication3d;
} // gui
} // bolt

#else

#error "bolt::gui was built without SDL3 or GLFW support"

#endif
