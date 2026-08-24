#include "gfx/vulkan/VulkanTexture.hpp"
#include "gfx/vulkan/VulkanRenderSystem.hpp"
#include "util/common.h"

#include <cstddef>
#include <cstring>
#include <stb_image.h>

namespace bolt {
namespace gfx {

VulkanTexture::VulkanTexture(const TextureDescriptor& desc, const VulkanRenderSystem* renderSystem) : mRenderSystem(renderSystem) {
    createTextureImage(desc);
    createTextureImageView();
}

VulkanTexture::~VulkanTexture() {
    vkDestroyImageView(mRenderSystem->device(), mTextureImageView, nullptr);
    vkDestroyImage(mRenderSystem->device(), mTextureImage, nullptr);
    vkFreeMemory(mRenderSystem->device(), mTextureImageMemory, nullptr);
}

void VulkanTexture::createTextureImage(const TextureDescriptor& desc) {
    int texWidth, texHeight, texChannels;
    stbi_uc* decoded = nullptr; // stb allocation, only when stb was the one to produce the texels
    const stbi_uc* pixels;
    if (desc.raw) {
        texWidth = static_cast<int>(desc.rawWidth);
        texHeight = static_cast<int>(desc.rawHeight);
        texChannels = 4;
        pixels = desc.raw->data();
    } else {
        decoded = desc.encoded ? stbi_load_from_memory(desc.encoded->data(),
                                                       static_cast<int>(desc.encoded->size()),
                                                       &texWidth, &texHeight, &texChannels,
                                                       STBI_rgb_alpha)
                               : stbi_load(desc.textureFile.c_str(), &texWidth, &texHeight,
                                           &texChannels, STBI_rgb_alpha);
        pixels = decoded;
    }
    RUNTIME_ASSERT(pixels != nullptr, "stbi: Failed to load texture image");
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    mRenderSystem->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    void* data;
    vkMapMemory(mRenderSystem->device(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(mRenderSystem->device(), stagingBufferMemory);
    if (decoded) {
        stbi_image_free(decoded);
    }

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
    mTextureImageView = mRenderSystem->createImageView(mTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}

} // gfx
} // bolt