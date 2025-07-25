#include "gfx/DrawableSpheroid.hpp"

#include "math/math.h"

namespace bolt {
namespace gfx {

static std::vector<PhongDrawableVertex> generateSphereVertices(float radiusX, float radiusY, float radiusZ, int slices, int stacks) {
    std::vector<PhongDrawableVertex> vertices;

    for (int i = 0; i <= stacks; ++i) {
        float phi = M_PI * i / stacks;
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);

        for (int j = 0; j <= slices; ++j) {
            float theta = 2 * M_PI * j / slices;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            math::Vector3f pos;
            pos.x = radiusX * sinPhi * cosTheta;
            pos.y = radiusY * cosPhi;
            pos.z = radiusZ * sinPhi * sinTheta;

            math::Vector3f normal = pos;
            normal.normalize();

            vertices.push_back({pos, normal});
        }
    }
    return vertices;
}

static std::vector<unsigned int> generateSphereIndices(int slices, int stacks) {
    std::vector<unsigned int> indices;

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int first = (i * (slices + 1)) + j;
            int second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    return indices;
}

DrawableSpheroid::DrawableSpheroid(float rX, float rY, float rZ, int slices, int stacks) {
    mVertices = generateSphereVertices(rX, rY, rZ, slices, stacks);
    mIndices = generateSphereIndices(slices, stacks);

    mMtx.setRotation(DEG2RAD(0), 0, 0);
    mMtx.setTranslation(0, 0, 0);
}

} // gfx
} // bolt
