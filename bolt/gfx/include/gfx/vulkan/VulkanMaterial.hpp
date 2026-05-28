#pragma once

#include <vulkan/vulkan.h>

namespace bolt {
namespace gfx {

/**
 * Represents a descriptor set that is shared between drawables within drawables drawn by the same pipeline (descriptor set 1)
 */
class VulkanMaterial {
    ~VulkanMaterial();
private:
    VkDescriptorSet mDescriptorSet;
};

} // gfx
} // bolt