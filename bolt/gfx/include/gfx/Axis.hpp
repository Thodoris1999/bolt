#pragma once

#include "gfx/SceneNode.hpp"
#include "gfx/DrawableLine.hpp"
#include "gfx/DrawableCone.hpp"

namespace bolt {
namespace gfx {

class Axis : public SceneNode {
public:
    Axis(float shaftLength, float tipLength, float tipDiameter, unsigned int samples = 8);

    void setColor(const Color& color) {
        mShaft.setColor(color);
        mTip.setAmbient(color);
    }

private:
    DrawableLine mShaft;
    DrawableCone mTip;
};

} // gfx
} // bolt
