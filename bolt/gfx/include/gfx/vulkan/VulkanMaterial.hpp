#pragma once

#include "gfx/vulkan/VulkanTexture.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace bolt {
namespace gfx {

class VulkanRenderSystem;

/**
 * Represents a descriptor set that is shared between drawables within drawables drawn by the same pipeline (descriptor set 1)
 */
class VulkanMaterial {
public:
    VulkanMaterial(const VulkanRenderSystem* renderSystem, const VkDescriptorSetLayout& layout,
        const std::vector<std::pair<uint32_t, VulkanTexture*>>& textures);

    VulkanMaterial(const VulkanMaterial&) = delete;
    VulkanMaterial& operator=(const VulkanMaterial&) = delete;

    VkDescriptorSet descriptorSet() const { return mDescriptorSet; }

private:
    const VulkanRenderSystem* mRenderSystem;
    VkDescriptorSet mDescriptorSet;
};

} // gfx
} // bolt