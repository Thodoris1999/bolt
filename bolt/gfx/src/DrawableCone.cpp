#include "gfx/DrawableCone.hpp"

#include <math.h>

namespace bolt {
namespace gfx {

DrawableCone::DrawableCone(float height, float radius, int segments) {
    mVertices.clear();
    mIndices.clear();

    const math::Vector3f tip = {0.0f, 0.0f, height};
    const math::Vector3f baseCenter = {0.0f, 0.0f, 0.0f};

    // Add tip vertex (index 0)
    mVertices.push_back({tip, {0.0f, 0.0f, 1.0f}});

    // Add base center vertex (index 1)
    mVertices.push_back({baseCenter, {0.0f, 0.0f, -1.0f}});

    // Base ring vertices start from index 2
    for (int i = 0; i < segments; ++i) {
        float angle = (2.0f * M_PI * i) / segments;
        float x = radius * std::cos(angle);
        float y = radius * std::sin(angle);
        math::Vector3f pos = {x, y, 0.0f};

        // Cone side normal (approximate)
        math::Vector3f sideNormal = {x, y, radius / height};
        sideNormal.normalize();

        mVertices.push_back({pos, sideNormal});
    }

    // --- Side triangles (tip to base ring) ---
    for (int i = 0; i < segments; ++i) {
        int current = 2 + i;
        int next = 2 + ((i + 1) % segments);

        mIndices.push_back(0);       // tip
        mIndices.push_back(current);
        mIndices.push_back(next);
    }

    // --- Base triangles (base center to base ring) ---
    for (int i = 0; i < segments; ++i) {
        int current = 2 + i;
        int next = 2 + ((i + 1) % segments);

        mIndices.push_back(1);       // base center
        mIndices.push_back(next);
        mIndices.push_back(current);
    }

    mMtx.setRotation(0, 0, 0);
    mMtx.setTranslation(0, 0, 0);
}

} // gfx
} // bolt
