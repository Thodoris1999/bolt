#include "gfx/vulkan/VulkanTexture.hpp"
#include "gfx/vulkan/VulkanRenderSystem.hpp"
#include "util/common.h"

#include <cstddef>
#include <stb_image.h>

namespace bolt {
namespace gfx {

VulkanTexture::VulkanTexture(const char* textureFile, const VulkanRenderSystem* renderSystem) : mRenderSystem(renderSystem) {
    createTextureImage(textureFile);
    createTextureImageView();
}

VulkanTexture::~VulkanTexture() {
    vkDestroyImageView(mRenderSystem->device(), mTextureImageView, nullptr);
    vkDestroyImage(mRenderSystem->device(), mTextureImage, nullptr);
    vkFreeMemory(mRenderSystem->device(), mTextureImageMemory, nullptr);
}

void VulkanTexture::createTextureImage(const char* textureFile) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(textureFile, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    RUNTIME_ASSERT(pixels != nullptr, "stbi: Failed to load texture image file");
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    mRenderSystem->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    void* data;
    vkMapMemory(mRenderSystem->device(), stagingBufferMemory, 0, imageSize, 0, &data);
    vkUnmapMemory(mRenderSystem->device(), stagingBufferMemory);
    stbi_image_free(pixels);

    mRenderSystem->createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        mTextureImage, mTextureImageMemory);

    mRenderSystem->transitionImageLayout(mTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    mRenderSystem->copyBufferToImage(stagingBuffer, mTextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    mRenderSystem->transitionImageLayout(mTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(mRenderSystem->device(), stagingBuffer, nullptr);
    vkFreeMemory(mRenderSystem->device(), stagingBufferMemory, nullptr);
}

void VulkanTexture::createTextureImageView() {
    mTextureImageView = mRenderSystem->createImageView(mTextureImage, VK_FORMAT_R8G8B8A8_SRGB);
}

} // gfx
} // bolt