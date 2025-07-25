#pragma once

#include "gfx/SceneNode.hpp"
#include "gfx/DrawableLine.hpp"
#include "gfx/DrawableCone.hpp"

namespace bolt {
namespace gfx {

class DrawableArrow3d : public SceneNode {
public:
    DrawableArrow3d(float shaftLength, float tipLength, float tipDiameter, unsigned int samples = 8);

    void setAmbient(const Color& color) {
        mShaft.setAmbient(color);
        mTip.setAmbient(color);
    }

private:
    DrawableLine mShaft;
    DrawableCone mTip;
};

} // gfx
} // bolt
