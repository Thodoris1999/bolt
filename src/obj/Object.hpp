#pragma once

#include "gfx/SceneNode.hpp"
#include "gfx/SceneManager.hpp"
#include "col/Collider.hpp"

/**
 * An Object is a convenience entity that acts as a handle for an entity that may have collision and/or visuals
 */
class Object3d {
public:
    Object3d();
    virtual ~Object3d();

    virtual void createVisuals(bolt::gfx::SceneManager& scene) {}

    bolt::gfx::SceneNode* sceneNode() { return mSceneNode; }
    bolt::col::Collider* collider() { return mCollider; }

    void setPose(bolt::math::Vector3f pos, bolt::math::Vector3f rot);

protected:
    bolt::gfx::SceneNode* mSceneNode;
    bolt::col::Collider* mCollider;
    bolt::math::Matrix34f mMtx;

private:
    void updateMatrix();
};
