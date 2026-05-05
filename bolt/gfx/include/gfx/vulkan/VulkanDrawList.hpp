#pragma once

#include "gfx/vulkan/VulkanDrawable.hpp"
#include "gfx/vulkan/VulkanProgram.hpp"

#include <vulkan/vulkan.h>

#include <unordered_map>

namespace bolt {
namespace gfx {

/**
 * Abstraction of a container holding a reference to all drawables. Internally maintains a representation such that 
 * it can register commands in an efficient manner (avoiding state and context switches etc)
 * 
 * Currently simply splits drawables into buckets based on the pipeline used. The public portion of this class can be turned into an abstract class to allow for
 * easily switchable different implementations
 */
class VulkanDrawList {
public:
    void addDrawable(VulkanDrawable* drawable);
    void recordCommands(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);

private:
    std::unordered_map<VulkanPipelineSignature, std::vector<VulkanDrawable*>, VulkanPipelineSignatureHash> mDrawables;
};

}
}