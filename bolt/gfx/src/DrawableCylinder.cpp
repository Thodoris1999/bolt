#include "gfx/DrawableCylinder.hpp"

#include <math.h>
#include <numbers>

namespace bolt {
namespace gfx {

DrawableCylinder::DrawableCylinder(float height, float radius, int segments) {
    mVertices.clear();
    mIndices.clear();

    float halfHeight = height * 0.5f;

    // Define centers
    const math::Vector3f topCenter    = {0.0f, 0.0f, +halfHeight};
    const math::Vector3f bottomCenter = {0.0f, 0.0f, -halfHeight};

    // --- Add center vertices ---
    int topCenterIndex    = 0;
    int bottomCenterIndex = 1;

    mVertices.push_back({topCenter,    {0.0f, 0.0f, 1.0f}});
    mVertices.push_back({bottomCenter, {0.0f, 0.0f, -1.0f}});

    // --- Top ring vertices start at index 2 ---
    int topRingStart = 2;

    for (int i = 0; i < segments; ++i) {
        float angle = (2.0f * std::numbers::pi * i) / segments;
        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);

        mVertices.push_back({{x, y, halfHeight}, {0.0f, 0.0f, 1.0f}});
    }

    // --- Bottom ring vertices ---
    int bottomRingStart = topRingStart + segments;

    for (int i = 0; i < segments; ++i) {
        float angle = (2.0f * std::numbers::pi * i) / segments;
        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);

        mVertices.push_back({{x, y, -halfHeight}, {0.0f, 0.0f, -1.0f}});
    }

    // --- Side ring vertices (for proper normal directions) ---
    int sideTopStart  = bottomRingStart + segments;
    int sideBottomStart = sideTopStart + segments;

    for (int i = 0; i < segments; ++i) {
        float angle = (2.0f * std::numbers::pi * i) / segments;
        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);

        math::Vector3f normal = {x, y, 0.0f};
        normal.normalize();

        mVertices.push_back({{x, y, +halfHeight}, normal});
    }

    for (int i = 0; i < segments; ++i) {
        float angle = (2.0f * std::numbers::pi * i) / segments;
        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);

        math::Vector3f normal = {x, y, 0.0f};
        normal.normalize();

        mVertices.push_back({{x, y, -halfHeight}, normal});
    }

    // ---------------- Indices ----------------

    // --- Top cap ---
    for (int i = 0; i < segments; ++i) {
        int current = topRingStart + i;
        int next = topRingStart + ((i + 1) % segments);

        mIndices.push_back(topCenterIndex);
        mIndices.push_back(current);
        mIndices.push_back(next);
    }

    // --- Bottom cap ---
    for (int i = 0; i < segments; ++i) {
        int current = bottomRingStart + i;
        int next = bottomRingStart + ((i + 1) % segments);

        mIndices.push_back(bottomCenterIndex);
        mIndices.push_back(next);
        mIndices.push_back(current);
    }

    // --- Side wall ---
    for (int i = 0; i < segments; ++i) {
        int topA    = sideTopStart + i;
        int topB    = sideTopStart + ((i + 1) % segments);
        int bottomA = sideBottomStart + i;
        int bottomB = sideBottomStart + ((i + 1) % segments);

        // Quad -> two triangles
        mIndices.push_back(topA);
        mIndices.push_back(bottomA);
        mIndices.push_back(bottomB);

        mIndices.push_back(topA);
        mIndices.push_back(bottomB);
        mIndices.push_back(topB);
    }

    mMtx.setRotation(0, 0, 0);
    mMtx.setTranslation(0, 0, 0);
}

} // gfx
} // bolt
