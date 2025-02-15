#pragma once

#include "Collider.hpp"

#include <vector>

namespace bolt {
namespace col {

/**
 * Simple collider container and collision query interface
 */
class ColliderManager {
public:
    void registerCollider(Collider* collider);
    bool queryCollision(const Collider* collider);

private:
    std::vector<Collider*> mColliders;
};

} // col
} // bolt