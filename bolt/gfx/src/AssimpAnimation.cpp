#include "gfx/AssimpAnimation.hpp"
#include "gfx/AssimpModel.hpp"
#include "gfx/SkinnedDrawable.hpp"

#include <assimp/scene.h>

#include <algorithm>
#include <cmath>

namespace bolt {
namespace gfx {

static math::Matrix44f toMatrix44f(const aiMatrix4x4& m) {
    math::Matrix44f result;
    result(0,0) = m.a1; result(0,1) = m.a2; result(0,2) = m.a3; result(0,3) = m.a4;
    result(1,0) = m.b1; result(1,1) = m.b2; result(1,2) = m.b3; result(1,3) = m.b4;
    result(2,0) = m.c1; result(2,1) = m.c2; result(2,2) = m.c3; result(2,3) = m.c4;
    result(3,0) = m.d1; result(3,1) = m.d2; result(3,2) = m.d3; result(3,3) = m.d4;
    return result;
}

static const aiNodeAnim* findChannel(const aiAnimation* animation, const std::string& nodeName) {
    for (unsigned int i = 0; i < animation->mNumChannels; i++) {
        aiNodeAnim* channel = animation->mChannels[i];
        if (nodeName == channel->mNodeName.C_Str()) {
            return channel;
        }
    }
    return nullptr;
}

// Resolves aiNodeAnim::mPreState/mPostState for a requested tick outside [0, animDuration]. Returns
// the tick to actually sample the channel's keys at, or a negative value if the channel's behaviour
// (aiAnimBehaviour_DEFAULT) says to fall back to the node's own bind-pose transform instead.
// aiAnimBehaviour_LINEAR is treated the same as aiAnimBehaviour_CONSTANT (clamped, not extrapolated).
static double resolveSampleTicks(const aiNodeAnim* channel, double ticks, double animDuration) {
    if (ticks >= 0.0 && ticks <= animDuration) {
        return ticks;
    }

    aiAnimBehaviour behaviour = ticks < 0.0 ? channel->mPreState : channel->mPostState;
    switch (behaviour) {
    case aiAnimBehaviour_REPEAT:
        if (animDuration <= 0.0) {
            return 0.0;
        }
        return std::fmod(std::fmod(ticks, animDuration) + animDuration, animDuration);
    case aiAnimBehaviour_CONSTANT:
    case aiAnimBehaviour_LINEAR:
        return ticks < 0.0 ? 0.0 : animDuration;
    case aiAnimBehaviour_DEFAULT:
    default:
        return -1.0;
    }
}

template <typename KeyT>
static unsigned int findKeyIndex(const KeyT* keys, unsigned int count, double ticks) {
    for (unsigned int i = 0; i + 1 < count; i++) {
        if (ticks < keys[i + 1].mTime) {
            return i;
        }
    }
    return count > 0 ? count - 1 : 0;
}

static math::Vector3f interpolatePosition(const aiNodeAnim* channel, double ticks) {
    if (channel->mNumPositionKeys == 1) {
        const aiVector3D& v = channel->mPositionKeys[0].mValue;
        return math::Vector3f(v.x, v.y, v.z);
    }

    unsigned int i0 = findKeyIndex(channel->mPositionKeys, channel->mNumPositionKeys, ticks);
    unsigned int i1 = std::min(i0 + 1, channel->mNumPositionKeys - 1);
    const auto& k0 = channel->mPositionKeys[i0];
    const auto& k1 = channel->mPositionKeys[i1];
    double dt = k1.mTime - k0.mTime;
    float t = dt > 0.0 ? static_cast<float>((ticks - k0.mTime) / dt) : 0.0f;

    math::Vector3f p0(k0.mValue.x, k0.mValue.y, k0.mValue.z);
    math::Vector3f p1(k1.mValue.x, k1.mValue.y, k1.mValue.z);
    return p0 + (p1 - p0) * t;
}

static math::Vector3f interpolateScale(const aiNodeAnim* channel, double ticks) {
    if (channel->mNumScalingKeys == 1) {
        const aiVector3D& v = channel->mScalingKeys[0].mValue;
        return math::Vector3f(v.x, v.y, v.z);
    }

    unsigned int i0 = findKeyIndex(channel->mScalingKeys, channel->mNumScalingKeys, ticks);
    unsigned int i1 = std::min(i0 + 1, channel->mNumScalingKeys - 1);
    const auto& k0 = channel->mScalingKeys[i0];
    const auto& k1 = channel->mScalingKeys[i1];
    double dt = k1.mTime - k0.mTime;
    float t = dt > 0.0 ? static_cast<float>((ticks - k0.mTime) / dt) : 0.0f;

    math::Vector3f s0(k0.mValue.x, k0.mValue.y, k0.mValue.z);
    math::Vector3f s1(k1.mValue.x, k1.mValue.y, k1.mValue.z);
    return s0 + (s1 - s0) * t;
}

// nlerp: a cheap slerp approximation (shortest-path lerp + normalize), good enough for preview playback
static math::Quatf interpolateRotation(const aiNodeAnim* channel, double ticks) {
    if (channel->mNumRotationKeys == 1) {
        const aiQuaternion& q = channel->mRotationKeys[0].mValue;
        return math::Quatf(q.x, q.y, q.z, q.w);
    }

    unsigned int i0 = findKeyIndex(channel->mRotationKeys, channel->mNumRotationKeys, ticks);
    unsigned int i1 = std::min(i0 + 1, channel->mNumRotationKeys - 1);
    const auto& k0 = channel->mRotationKeys[i0];
    const auto& k1 = channel->mRotationKeys[i1];
    double dt = k1.mTime - k0.mTime;
    float t = dt > 0.0 ? static_cast<float>((ticks - k0.mTime) / dt) : 0.0f;

    float x0 = k0.mValue.x, y0 = k0.mValue.y, z0 = k0.mValue.z, w0 = k0.mValue.w;
    float x1 = k1.mValue.x, y1 = k1.mValue.y, z1 = k1.mValue.z, w1 = k1.mValue.w;
    if (x0 * x1 + y0 * y1 + z0 * z1 + w0 * w1 < 0.0f) {
        x1 = -x1; y1 = -y1; z1 = -z1; w1 = -w1;
    }

    float x = x0 + (x1 - x0) * t;
    float y = y0 + (y1 - y0) * t;
    float z = z0 + (z1 - z0) * t;
    float w = w0 + (w1 - w0) * t;
    float len = std::sqrt(x * x + y * y + z * z + w * w) + 1e-8f;
    return math::Quatf(x / len, y / len, z / len, w / len);
}

static math::Matrix44f nodeLocalTransform(const aiNodeAnim* channel, double ticks) {
    math::Vector3f scale = interpolateScale(channel, ticks);
    math::Quatf rotation = interpolateRotation(channel, ticks);
    math::Vector3f translation = interpolatePosition(channel, ticks);

    math::Matrix44f scaleMtx;
    scaleMtx.setIdentity();
    scaleMtx(0, 0) = scale.x;
    scaleMtx(1, 1) = scale.y;
    scaleMtx(2, 2) = scale.z;

    math::Matrix44f rotationMtx;
    rotationMtx.setIdentity();
    rotationMtx.setRotation(rotation);

    math::Matrix44f result = rotationMtx * scaleMtx;
    result.setTranslation(translation);
    return result;
}

AssimpAnimation::AssimpAnimation(AssimpModel& model, const aiAnimation* animation) :
        mModel(model), mAnimation(animation), mName(animation->mName.C_Str()) {
    mTicksPerSecond = animation->mTicksPerSecond != 0.0 ? animation->mTicksPerSecond : 25.0;
    mDurationSeconds = animation->mDuration / mTicksPerSecond;
    mGlobalInverseTransform = toMatrix44f(mModel.scene()->mRootNode->mTransformation).rigidInverse();
}

void AssimpAnimation::setTime(double seconds) {
    double ticks = seconds * mTicksPerSecond;

    math::Matrix44f identity;
    identity.setIdentity();
    applyNode(mModel.scene()->mRootNode, identity, ticks);
}

void AssimpAnimation::applyNode(const aiNode* node, const math::Matrix44f& parentTransform, double ticks) {
    std::string nodeName(node->mName.C_Str());
    const aiNodeAnim* channel = findChannel(mAnimation, nodeName);

    math::Matrix44f nodeTransform;
    if (channel == nullptr) {
        nodeTransform = toMatrix44f(node->mTransformation);
    } else {
        double sampleTicks = resolveSampleTicks(channel, ticks, mAnimation->mDuration);
        nodeTransform = sampleTicks < 0.0 ? toMatrix44f(node->mTransformation) : nodeLocalTransform(channel, sampleTicks);
    }

    math::Matrix44f globalTransform = parentTransform * nodeTransform;

    const math::Matrix44f* offset = mModel.boneOffset(nodeName);
    if (offset != nullptr) {
        math::Matrix44f finalTransform = mGlobalInverseTransform * globalTransform * (*offset);
        for (SkinnedDrawable* mesh : mModel.skinnedMeshes()) {
            if (mesh->hasBone(nodeName)) {
                mesh->setBoneTransform(nodeName, finalTransform);
            }
        }
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        applyNode(node->mChildren[i], globalTransform, ticks);
    }
}

} // gfx
} // bolt
