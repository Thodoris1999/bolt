#pragma once

#include "math/Matrix.hpp"

#include <string>

struct aiAnimation;
struct aiNode;
struct aiNodeAnim;

namespace bolt {
namespace gfx {

class AssimpModel;

class AssimpAnimation {
public:
    AssimpAnimation(AssimpModel& model, const aiAnimation* animation);

    const std::string& name() const { return mName; }
    /// Duration of the animation's own defined keyframe range, in seconds.
    double durationSeconds() const { return mDurationSeconds; }

    /// Sets every bone this animation drives to its pose at \p seconds. Only affects bones present in mModel.
    void setTime(double seconds);

private:
    void applyNode(const aiNode* node, const math::Matrix44f& parentTransform, double ticks);

    AssimpModel& mModel;
    const aiAnimation* mAnimation;
    std::string mName;
    double mTicksPerSecond;
    double mDurationSeconds;
    math::Matrix44f mGlobalInverseTransform;
};

} // gfx
} // bolt
