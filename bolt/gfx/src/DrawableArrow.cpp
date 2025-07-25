#include "gfx/DrawableArrow.hpp"

namespace bolt {
namespace gfx {

DrawableArrow3d::DrawableArrow3d(float shaftLength, float tipLength, float tipDiameter, unsigned int samples)
    : mShaft({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, shaftLength}),
      mTip(tipLength, tipDiameter * 0.5f, samples) {

    // Position the tip at the end of the shaft (along +Z)
    math::Matrix34f tipTransform;
    tipTransform.setRotation(0, 0, 0);
    tipTransform.setTranslation(0.0f, 0.0f, shaftLength);

    mTip.setMtx(tipTransform);

    addChild(&mShaft);
    addChild(&mTip);
}

} // gfx
} // bolt
