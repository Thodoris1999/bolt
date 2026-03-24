#pragma once

#include "gfx/RenderProgram.hpp"
#include "gfx/Drawable.hpp"

#include <vulkan/vulkan.h>

namespace bolt {
namespace gfx {

struct VulkanPipelineSignature {
    ProgramDescriptor programDescriptor;
    PrimitiveType primitiveType;

    bool operator==(const VulkanPipelineSignature& other) const {
        return programDescriptor == other.programDescriptor &&
            primitiveType == other.primitiveType;
    }
};
struct VulkanPipelineSignatureHash {
    size_t operator()(const VulkanPipelineSignature& sig) const {
        size_t h1 = std::hash<std::string>{}(sig.programDescriptor.vertShader);
        size_t h2 = std::hash<std::string>{}(sig.programDescriptor.fragShader);
        size_t h3 = std::hash<int>{}(static_cast<int>(sig.primitiveType));

        size_t combined = h1;
        combined ^= h2 + 0x9e3779b9 + (combined << 6) + (combined >> 2);
        combined ^= h3 + 0x9e3779b9 + (combined << 6) + (combined >> 2);
        return combined;
    }
};

/**
 * Abstraction over a vulkan pipeline, exposing runtime dependent settings and data transfer
 */
class VulkanProgram : public RenderProgram {
public:
    VulkanProgram(VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, Drawable* d);
    virtual ~VulkanProgram() {
        vkDestroyPipeline(mDevice, mGraphicsPipeline, nullptr);
    }

    VkPipeline pipeline() { return mGraphicsPipeline; }

private:
    VkDevice mDevice;
    VkPipeline mGraphicsPipeline;
};

}
}