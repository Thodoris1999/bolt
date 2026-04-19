#pragma once

#include "util/Filesystem.hpp"
#include "util/common.h"

#include "gl_defines.h"

#include <cstdlib>

namespace bolt {
namespace gfx {

void openglDebugOutputCallback(GLenum source,
                            GLenum type,
                            unsigned int id,
                            GLenum severity,
                            GLsizei length,
                            const char *message,
                            const void *userParam);

} // gfx
} // bolt
