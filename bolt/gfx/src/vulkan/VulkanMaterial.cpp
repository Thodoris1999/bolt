#include "gfx/vulkan/VulkanMaterial.hpp"
#include "gfx/vulkan/VulkanRenderSystem.hpp"
#include "util/common.h"
#include <vulkan/vulkan_core.h>

namespace bolt {
namespace gfx {

VulkanMaterial::VulkanMaterial(const VulkanRenderSystem* renderSystem, const std::vector<VulkanTexture*> textures, VkSampler textureSampler) {
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
        const VulkanTexture* texture = textures[i];
        materialImageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        materialImageInfo[i].imageView = textureImageView;
        materialImageInfo[i].sampler = textureSampler;

        materialDescriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        materialDescriptorWrites[i].dstSet = mDescriptorSet;
        materialDescriptorWrites[i].dstBinding = 1;
        materialDescriptorWrites[i].dstArrayElement = 0;
        materialDescriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        materialDescriptorWrites[i].descriptorCount = 1;
        materialDescriptorWrites[i].pImageInfo = &materialImageInfo[i];
    }

    vkUpdateDescriptorSets(renderSystem->device(), static_cast<uint32_t>(materialDescriptorWrites.size()), materialDescriptorWrites.data(), 0, nullptr);
}

VulkanMaterial::~VulkanMaterial() {
    vkDestroyDescriptorSet(mRenderSystem->device(), mLayout, nullptr);
}

}
}