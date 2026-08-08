#pragma once

#include "gfx/RenderUniformBuffer.hpp"

#include <vulkan/vulkan.h>

#include <stdint.h>
#include <vector>

namespace bolt {
namespace gfx {

class VulkanRenderSystem;

class VulkanUniformBuffer : public RenderUniformBuffer {
public:
    VulkanUniformBuffer(const VulkanRenderSystem* renderSystem, size_t size, uint32_t bindPoint);
    virtual ~VulkanUniformBuffer();

    virtual void writeData(const void* src, size_t offset, size_t size) override;

    void createBuffers(int numFrames);
    void writeDescriptorSet(VkDescriptorSet dstSet, int frameIndex) const;
    /// copies staging data into the given frame's mapped buffer, but only if
    /// writeData() has been called since that frame was last updated
    void update(int currentImage);

    uint32_t bindPoint() const { return mBindPoint; }
    VkShaderStageFlags stageFlags() const { return mStageFlags; }

private:
    const VulkanRenderSystem* mRenderSystem;
    VkDeviceSize mSize;
    uint32_t mBindPoint;
    VkShaderStageFlags mStageFlags;
    std::vector<VkBuffer> mUniformBuffers;
    std::vector<VkDeviceMemory> mUniformBuffersMemory;
    std::vector<void*> mUniformBuffersMapped;
    std::vector<bool> mDirty; // per frame-in-flight; true if staging data hasn't been applied to that frame's buffer yet
    void* mStagingBuffer;
};

} // gfx
} // bolt
