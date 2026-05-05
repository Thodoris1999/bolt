#pragma once

#include "gfx/Drawable.hpp"

#include <vulkan/vulkan.h>

namespace bolt {
namespace gfx {

class VulkanRenderSystem;

/**
 * Class representing an item in the command queue to be drawn
 */
class VulkanDrawable {
public:
    VulkanDrawable(VulkanRenderSystem* renderSystem, Drawable* drawable) : mRenderSystem(renderSystem), drawable(drawable), mLoaded(false) {};
    ~VulkanDrawable() { if (mLoaded) unload(); }

    /// create and load buffers, textures, uniforms etc
    void load();
    /// clear resources created by load
    void unload();

    VkBuffer vertexBuffer() { return mVertexBuffer; }
    VkBuffer indexBuffer() { return mIndexBuffer; }

    Drawable* drawable;

private:
    void createVertexBuffer();
    void createIndexBuffer();

    VulkanRenderSystem* mRenderSystem;
    VkBuffer mVertexBuffer;
    VkDeviceMemory mVertexBufferMemory;
    VkBuffer mIndexBuffer;
    VkDeviceMemory mIndexBufferMemory;
    void* mPushConstantBuffer;
    bool mLoaded;
};

}
}