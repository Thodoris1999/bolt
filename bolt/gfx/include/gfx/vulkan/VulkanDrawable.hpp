#pragma once

#include "gfx/Drawable.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <cassert>

namespace bolt {
namespace gfx {

struct DrawKey
{
    uint64_t value = 0;

    // --- packing ---
    static constexpr uint64_t PIPELINE_SHIFT = 32;
    static constexpr uint64_t PIPELINE_MASK  = 0xFFFFFFFFull;
    static constexpr uint64_t MATERIAL_MASK  = 0xFFFFFFFFull;

    static uint64_t make(uint32_t pipelineID, uint32_t materialID)
    {
        return (uint64_t(pipelineID) << PIPELINE_SHIFT) |
               (uint64_t(materialID) & MATERIAL_MASK);
    }

    // --- unpacking ---
    static uint32_t getPipeline(uint64_t key)
    {
        return uint32_t((key >> PIPELINE_SHIFT) & PIPELINE_MASK);
    }

    static uint32_t getMaterial(uint64_t key)
    {
        return uint32_t(key & MATERIAL_MASK);
    }

    // --- convenience wrappers ---
    static uint64_t setPipeline(uint64_t key, uint32_t pipelineID)
    {
        return (key & 0x00000000FFFFFFFFull) |
               (uint64_t(pipelineID) << PIPELINE_SHIFT);
    }

    static uint64_t setMaterial(uint64_t key, uint32_t materialID)
    {
        return (key & 0xFFFFFFFF00000000ull) |
               uint64_t(materialID);
    }

    constexpr auto operator<=>(const DrawKey&) const = default;
};

class VulkanRenderSystem;

/**
 * Class representing an item in the command queue to be drawn
 */
class VulkanDrawable {
public:
    VulkanDrawable(VulkanRenderSystem* renderSystem, Drawable* drawable) : drawable(drawable), mRenderSystem(renderSystem), mLoaded(false) {};
    ~VulkanDrawable() { if (mLoaded) unload(); }

    VulkanDrawable(const VulkanDrawable&) = delete;
    VulkanDrawable& operator=(const VulkanDrawable&) = delete;

    /// create and load buffers, textures, uniforms etc
    void load();
    /// clear resources created by load
    void unload();

    void setKey(uint32_t pipelineID, uint32_t materialID) {
        drawKey = DrawKey::make(pipelineID, materialID);
    }

    VkBuffer vertexBuffer() const { return mVertexBuffer; }
    VkBuffer indexBuffer() const { return mIndexBuffer; }

    Drawable* drawable;
    uint64_t drawKey;

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