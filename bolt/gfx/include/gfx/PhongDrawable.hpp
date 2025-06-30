#pragma once

#include "Drawable.hpp"
#include "Color.hpp"

namespace bolt {
namespace gfx {

class PhongDrawable : public Drawable3d {
public:
    PhongDrawable();

    Color& ambient() { return mAmbient; }
    void setAmbient(const Color& color);

protected:
    Color mAmbient;
    Color mDiffuse;
    Color mSpecular;
};

} // gfx
} // bolt
