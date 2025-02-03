#include "ColliderManager.hpp"
#include "ColliderConvex.hpp"

#include "common.h"

static bool dispatchCollision(const Collider* a, const Collider* b) {
    if (a->isConvex() && b->isConvex()) {
        return checkConvexCollision(*static_cast<const ColliderConvex*>(a), *static_cast<const ColliderConvex*>(b));
    } else {
        RUNTIME_ASSERT(false, "dispatcher not supporting given colliders");
        return false;
    }
}

void ColliderManager::registerCollider(Collider* collider) {
    mColliders.push_back(collider);
}

bool ColliderManager::queryCollision(const Collider* collider) {
    for (const auto* coll : mColliders) {
        // do not collide with self
        if (collider == coll) continue;

        if (dispatchCollision(collider, coll)) return true;
    }

    return false;
}
