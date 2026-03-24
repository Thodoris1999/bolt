#pragma once

#include "gfx/RenderUniformBuffer.hpp"

#include <vulkan/vulkan.h>

#include <stdint.h>

namespace bolt {
namespace gfx {

class VulkanRenderSystem;

class VulkanUniformBuffer : public RenderUniformBuffer {
public:
    VulkanUniformBuffer(const VulkanRenderSystem* renderSystem, size_t size, uint32_t bindPoint);
    virtual ~VulkanUniformBuffer();

    virtual void writeData(const void* src, size_t offset, size_t size) override;

    void createBuffers(int numFrames);
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
    void* mStagingBuffer;
};

} // gfx
} // bolt
