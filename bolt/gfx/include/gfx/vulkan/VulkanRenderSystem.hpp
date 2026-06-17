#pragma once

#include "gfx/Drawable.hpp"
#include "gfx/RenderSystem.hpp"
#include "gfx/vulkan/VulkanDrawable.hpp"
#include "gfx/vulkan/VulkanMaterial.hpp"
#include "gfx/vulkan/VulkanProgram.hpp"
#include "gfx/vulkan/VulkanDrawList.hpp"
#include "gfx/vulkan/VulkanTexture.hpp"

#include <vector>
#include <vulkan/vulkan.h>

#include <unordered_map>
#include <array>
#include <memory>
#include <optional>

namespace bolt {
namespace gfx {

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

/**
 * Callback used by the engine to query viewport/framebuffer size
 */
using GetFramebufferSizeFn = void(*)(void* userData, uint32_t& width, uint32_t& height);

struct WindowHooks {
    GetFramebufferSizeFn getFramebufferSize;
    void* userData;
};

struct QueueFamilyIndices {
    // GRAPHICS_BIT
    std::optional<uint32_t> graphicsFamily;
    // supports KHR surface
    std::optional<uint32_t> presentFamily;

    bool isComplete(bool headless) {
        return graphicsFamily.has_value() && (headless || presentFamily.has_value());
    }
};

/**
 * A vulkan rendering backend
 * 
 * For initialization, follow the following steps:
 * 1. Construct a VulkanRenderSystem
 * 2. Set the desired surface by calling surface() and setting it to the one provided by your window system
 * 3. Call init(), passing the width and height of your window in pixels
 * Then you can use it as a regular RenderSystem
 */
class VulkanRenderSystem final : public RenderSystem {
public:
    VulkanRenderSystem(const char* const * extensions, uint32_t extensionCount, const WindowHooks& windowHooks);
    /// headless constructor: no window/surface/swapchain, renders into a self-owned offscreen target
    VulkanRenderSystem(uint32_t offscreenWidth, uint32_t offscreenHeight);
    virtual ~VulkanRenderSystem();

    VkInstance& instance() { return mInstance; }
    VkSurfaceKHR& surface() { return mSurface; }
    void init();

    virtual void setClearColor(float r, float g, float b, float a) override;
    virtual void setViewport(int x, int y, int width, int height) override;
    virtual void addDrawable(Drawable* drawable) override;
    virtual RenderUniformBuffer* addUniform(size_t size, uint32_t bindPoint) override;
    virtual void load() override;
    virtual void renderFrame() override;
    virtual math::Matrix44f clipSpaceCorrection() const override;

    VkDevice device() const { return mDevice; }
    VkCommandPool commandPool() { return mCommandPool; }
    VkDescriptorPool descriptorPool() const { return mDescriptorPool; }
    VkDescriptorSet sceneDescriptorSet(int i) const { return mSceneDescriptorSets[i]; }
    VkDescriptorSetLayout sceneDescriptorSetLayout() const { return mSceneDescriptorSetLayout; }
    VkRenderPass renderPass() const { return mRenderPass; }
    VkSampler textureSampler() const { return mTextureSampler; }
    const VulkanProgram& getProgram(uint32_t id) const { return *mPrograms[id]; }
    const VulkanMaterial& getMaterial(uint32_t id) const { return *mMaterials[id]; }

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) const;
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const;
    VkCommandBuffer beginSingleTimeCommands() const;
    void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;
    /// copy buffer (blocking)
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    // transition image layout (blocking)
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const;
    // copy buffer to image (blocking)
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;
    /// (headless only) blocking readback of the most recently rendered frame into dst, RGBA8, width*height*4 bytes
    void readFramebuffer(void* dst);

private:
    constexpr static int MAX_FRAMES_IN_FLIGHT = 2;

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    void createInstance(const char* const * extensions, uint32_t extensionCount);
    void setupPhysicalDevice();
    void createLogicalDevice();
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    void createSwapChain();
    void createImageViews();
    void createOffscreenTarget();
    void createDescriptorPool();
    void createDescriptorSets();
    uint32_t registerProgram(Drawable* d);
    VulkanTexture* registerTexture(Drawable* d, const TextureDescriptor& textureDescriptor);
    uint32_t registerMaterial(Drawable* d, const VkDescriptorSetLayout layout, std::vector<std::pair<uint32_t, VulkanTexture*>> textureBindings);
    void createRenderPass();
    void createDepthResources();
    void createFramebuffers();
    void createTextureSampler();
    void createCommandPool();
    void createCommandBuffer();
    void createSyncObjects();
    void createRenderFinishedSemaphores();
    void destroyRenderFinishedSemaphores();
    void createPushConstantBuffer();
    void createSceneDescriptorSetLayout();

    void recreateSwapChain();
    void cleanupSwapChain();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, const VkDescriptorSet& sceneSet, uint32_t imageIndex);
    void renderFrameHeadless();

    VkInstance mInstance;
    VkSurfaceKHR mSurface;
    VkPhysicalDevice mPhysicalDevice;
    VkDevice mDevice;
    const bool mHeadless;
    const std::vector<const char*> mDeviceExtensions;
    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;
    VkSwapchainKHR mSwapChain;
    std::vector<VkImage> mSwapChainImages;
    std::vector<VkDeviceMemory> mOffscreenImageMemories; // headless only: self-allocated mSwapChainImages need explicit memory
    VkBuffer mReadbackBuffer; // headless only: persistent staging buffer reused by readFramebuffer() every frame
    VkDeviceMemory mReadbackBufferMemory;
    void* mReadbackBufferMapped;
    VkCommandBuffer mReadbackCommandBuffer; // headless only: persistent command buffer reused by readFramebuffer() every frame
    VkFence mReadbackFence;
    VkFormat mSwapChainImageFormat;
    VkExtent2D mSwapChainExtent;
    std::vector<VkImageView> mSwapChainImageViews;

    VkRenderPass mRenderPass;
    VkImage mDepthImage;
    VkDeviceMemory mDepthImageMemory;
    VkImageView mDepthImageView;
    VkClearValue mClearColor;
    std::vector<VkFramebuffer> mSwapChainFramebuffers;
    VkCommandPool mCommandPool;
    VkDescriptorSetLayout mSceneDescriptorSetLayout;
    VkSampler mTextureSampler;
    VkPushConstantRange mPushConstantRange;
    void* mPushConstantBuffer;
    VkDescriptorPool mDescriptorPool;
    std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> mSceneDescriptorSets;
    std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> mCommandBuffers;
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> mImageAvailableSemaphores;
    std::vector<VkSemaphore> mRenderFinishedSemaphores;
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT> mInFlightFences;
    WindowHooks mWindowHooks;
    uint32_t mCurrentFrame;
    bool mFramebufferResized;
    int mLastRenderedImageIndex; // headless only: which mSwapChainImages slot readFramebuffer() should read

    std::vector<std::unique_ptr<VulkanDrawable>> mDrawables;

    std::unordered_map<std::string, VulkanTexture*> mTextures;
    std::vector<std::unique_ptr<VulkanProgram>> mPrograms;
    std::unordered_map<VulkanPipelineSignature, uint32_t, VulkanPipelineSignatureHash> mProgramMap;
    std::vector<std::unique_ptr<VulkanMaterial>> mMaterials;
    std::unordered_map<Material, uint32_t, MaterialHash> mMaterialMap;
    VulkanDrawList mDrawList;
};

} // gfx
} // bolt