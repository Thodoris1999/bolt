#pragma once

#include "gfx/RenderSystem.hpp"
#include "gfx/vulkan/VulkanDrawable.hpp"
#include "gfx/vulkan/VulkanProgram.hpp"
#include "gfx/vulkan/VulkanDrawList.hpp"

#include <vulkan/vulkan.h>

#include <unordered_map>
#include <array>

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
    virtual ~VulkanRenderSystem();

    VkInstance& instance() { return mInstance; }
    VkSurfaceKHR& surface() { return mSurface; }
    void init();

    virtual void setClearColor(float r, float g, float b, float a);
    virtual void setViewport(int x, int y, int width, int height) override;
    virtual void addDrawable(Drawable* drawable) override;
    virtual RenderUniformBuffer* addUniform(size_t size, uint32_t bindPoint) override;
    virtual void load() override;
    virtual void renderFrame() override;

    VkDevice device() { return mDevice; }
    VkCommandPool commandPool() { return mCommandPool; }

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    /// copy buffer (blocking)
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

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
    void registerProgram(Drawable* d);
    void createRenderPass();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffer();
    void createSyncObjects();

    void recreateSwapChain();
    void cleanupSwapChain();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    VkInstance mInstance;
    VkSurfaceKHR mSurface;
    VkPhysicalDevice mPhysicalDevice;
    VkDevice mDevice;
    const std::vector<const char*> mDeviceExtensions;
    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;
    VkSwapchainKHR mSwapChain;
    std::vector<VkImage> mSwapChainImages;
    VkFormat mSwapChainImageFormat;
    VkExtent2D mSwapChainExtent;
    std::vector<VkImageView> mSwapChainImageViews;
    VkRenderPass mRenderPass;
    VkClearValue mClearColor;
    std::vector<VkFramebuffer> mSwapChainFramebuffers;
    VkCommandPool mCommandPool;
    std::array<VkCommandBuffer, MAX_FRAMES_IN_FLIGHT> mCommandBuffers;
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> mImageAvailableSemaphores;
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> mRenderFinishedSemaphores;
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT> mInFlightFences;
    WindowHooks mWindowHooks;
    uint32_t mCurrentFrame;
    bool mFramebufferResized;

    std::vector<VulkanDrawable> mDrawables;

    std::unordered_map<VulkanPipelineSignature, RenderProgram*, VulkanPipelineSignatureHash> mPrograms;
    VulkanDrawList mDrawList;
};

} // gfx
} // bolt