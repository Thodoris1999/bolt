#include "gfx/vulkan/VulkanMaterial.hpp"
#include "gfx/vulkan/VulkanRenderSystem.hpp"
#include "util/common.h"

namespace bolt {
namespace gfx {

VulkanMaterial::VulkanMaterial(const VulkanRenderSystem* renderSystem, const VkDescriptorSetLayout& layout,
        const std::vector<std::pair<uint32_t, VulkanTexture*>>& textures) : mRenderSystem(renderSystem), mDescriptorSet(VK_NULL_HANDLE) {
    // programs that declare no set=1 bindings (no per-material textures) have no layout to allocate against
    if (layout == VK_NULL_HANDLE) {
        return;
    }

    // create descriptor set
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = renderSystem->descriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    VkResult result = vkAllocateDescriptorSets(renderSystem->device(), &allocInfo, &mDescriptorSet);
    RUNTIME_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to create material descriptor set");

    //update descriptor set
    std::vector<VkDescriptorImageInfo> materialImageInfo(textures.size());
    std::vector<VkWriteDescriptorSet> materialDescriptorWrites(textures.size());
    for (int i = 0; i < (int)textures.size(); i++) {
        const std::pair<uint32_t, VulkanTexture*> texture = textures[i];
        materialImageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        materialImageInfo[i].imageView = texture.second->textureImageView();
        materialImageInfo[i].sampler = renderSystem->textureSampler();

        materialDescriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        materialDescriptorWrites[i].dstSet = mDescriptorSet;
        materialDescriptorWrites[i].dstBinding = texture.first;
        materialDescriptorWrites[i].dstArrayElement = 0;
        materialDescriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        materialDescriptorWrites[i].descriptorCount = 1;
        materialDescriptorWrites[i].pImageInfo = &materialImageInfo[i];
    }

    vkUpdateDescriptorSets(renderSystem->device(), static_cast<uint32_t>(materialDescriptorWrites.size()), materialDescriptorWrites.data(), 0, nullptr);
}

}
}