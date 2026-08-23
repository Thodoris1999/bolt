#pragma once

#include "gfx/Drawable.hpp"
#include "gfx/RenderTexture.hpp"

#include <vulkan/vulkan.h>

namespace bolt {
namespace gfx {

class VulkanRenderSystem;
class VulkanTexture final : public RenderTexture {
public:
    VulkanTexture(const TextureDescriptor& desc, const VulkanRenderSystem* renderSystem);
    virtual ~VulkanTexture();

    VkImageView textureImageView() const { return mTextureImageView; }

private:
    void createTextureImage(const TextureDescriptor& desc);
    void createTextureImageView();

    const VulkanRenderSystem* mRenderSystem;
    VkImage mTextureImage;
    VkImageView mTextureImageView;
    VkDeviceMemory mTextureImageMemory;
};

} // gfx
} // bolt
