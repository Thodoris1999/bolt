#pragma once

#include "util/Export.hpp"

namespace bolt {
namespace gfx {

struct Color {
    float r;
    float g;
    float b;
    float a;

    Color() = default;
    Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
};

BOLT_GFX_DATA extern const Color COLOR_WHITE;
BOLT_GFX_DATA extern const Color COLOR_BLACK;
BOLT_GFX_DATA extern const Color COLOR_RED;
BOLT_GFX_DATA extern const Color COLOR_GREEN;
BOLT_GFX_DATA extern const Color COLOR_BLUE;

} // gfx
} // bolt
