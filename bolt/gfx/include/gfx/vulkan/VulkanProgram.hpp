#pragma once

#include "gfx/RenderProgram.hpp"
#include "gfx/Drawable.hpp"

#include <vulkan/vulkan.h>

namespace bolt {
namespace gfx {

struct VulkanPipelineSignature {
    csp::ProgramDescriptor programDescriptor;
    PrimitiveType primitiveType;

    bool operator==(const VulkanPipelineSignature& other) const {
        const auto& lhs = programDescriptor;
        const auto& rhs = other.programDescriptor;

        // Compare Vk shader source filenames
        if (lhs.source_count != rhs.source_count) {
            return false;
        }
        for (uint32_t i = 0; i < lhs.source_count; ++i) {
            const auto& a = lhs.vk_sources[i];
            const auto& b = rhs.vk_sources[i];

            if (a.filename != b.filename) {
                return false;
            }
        }

        // Compare primitive type
        return primitiveType == other.primitiveType;
    }
};
struct VulkanPipelineSignatureHash {
    size_t operator()(const VulkanPipelineSignature& sig) const {
        const auto& p = sig.programDescriptor;

        std::size_t seed = 0;
        std::hash<std::string_view> str_hash;

        // Hash all shader filenames (order matters)
        for (uint32_t i = 0; i < p.source_count; ++i) {
            std::size_t h = str_hash(p.vk_sources[i].filename);

            seed ^= h + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
        }

        // Hash primitive type
        std::size_t prim_hash = std::hash<int>{}(static_cast<int>(sig.primitiveType));
        seed ^= prim_hash + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);

        return seed;
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