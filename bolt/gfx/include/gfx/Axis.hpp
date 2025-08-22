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
        math::Vector3f colorVec(color.r, color.g, color.b);
        mTip.setAmbient(colorVec);
    }

private:
    DrawableLine mShaft;
    DrawableCone mTip;
};

} // gfx
} // bolt
