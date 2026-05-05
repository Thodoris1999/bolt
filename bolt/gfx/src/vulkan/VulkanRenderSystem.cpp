#include "gfx/vulkan/VulkanRenderSystem.hpp"
#include "gfx/vulkan/VulkanProgram.hpp"
#include "gfx/vulkan/VulkanUniformBuffer.hpp"

#include <vulkan/vulkan.h>

#include <cstring>
#include <optional>
#include <set>
#include <algorithm>

namespace bolt {
namespace gfx {

static bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

static bool checkDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

struct QueueFamilyIndices {
    // GRAPHICS_BIT
    std::optional<uint32_t> graphicsFamily;
    // supports KHR surface
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

QueueFamilyIndices VulkanRenderSystem::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

bool VulkanRenderSystem::isDeviceSuitable(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device, mDeviceExtensions);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && indices.isComplete() && extensionsSupported && swapChainAdequate;
}

VulkanRenderSystem::VulkanRenderSystem(const char* const * extensions, uint32_t extensionCount, const WindowHooks& windowHooks) :
mWindowHooks(windowHooks),
mDeviceExtensions({VK_KHR_SWAPCHAIN_EXTENSION_NAME}),
mCurrentFrame(0), mFramebufferResized(false) {
    createInstance(extensions, extensionCount);
    mSurface = VK_NULL_HANDLE;
    mPipelineLayout = VK_NULL_HANDLE;
    mDescriptorPool = VK_NULL_HANDLE;
    mDescriptorSetLayout = VK_NULL_HANDLE;
    mPushConstantBuffer = malloc(0); // just so that free works in the constructor even if load was never called
}

void VulkanRenderSystem::init() {
    setupPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    createFramebuffers();
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
}

VulkanRenderSystem::~VulkanRenderSystem() {
    vkDeviceWaitIdle(mDevice); // flush queued operations (before cleaning up anything else!)

    for (auto& d : mDrawables) {
        //  clear buffers, textures, uniforms etc
        d.unload();
    }
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(mDevice, mImageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[i], nullptr);
        vkDestroyFence(mDevice, mInFlightFences[i], nullptr);
    }
    vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
    for (auto& entry : mPrograms) { // deletes pipelines and pipeline layouts
        delete entry.second;
    }
    vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
    cleanupSwapChain();
    free(mPushConstantBuffer);
    vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
    for (auto* entry : mUniforms) {
        delete entry;
    }
    vkDestroyDescriptorPool(mDevice, mDescriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(mDevice, mDescriptorSetLayout, nullptr);
    vkDestroyDevice(mDevice, nullptr);
    vkDestroyInstance(mInstance, nullptr);
}

void VulkanRenderSystem::createInstance(const char* const * extensions, uint32_t extensionCount) {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "bolt gfx vulkan render backend";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "bolt";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = extensionCount;
    createInfo.ppEnabledExtensionNames = extensions;
#ifndef NDEBUG
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    RUNTIME_ASSERT(checkValidationLayerSupport(validationLayers), "Failed to load requested vulkan validation layers");
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
#else
    createInfo.enabledLayerCount = 0;
#endif

    VkResult result = vkCreateInstance(&createInfo, nullptr, &mInstance);
    RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create vulkan instance");
}

void VulkanRenderSystem::setupPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);
    RUNTIME_ASSERT(deviceCount > 0, "Could not find any devices with Vulkan support");
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            mPhysicalDevice = device;
            break;
        }
    }

    RUNTIME_ASSERT(mPhysicalDevice != VK_NULL_HANDLE, "Failed to find a suitable Vulkan GPU");
}

void VulkanRenderSystem::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(mDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = mDeviceExtensions.data();


    VkResult result = vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice);
    RUNTIME_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to create logical device");

    vkGetDeviceQueue(mDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);
}

SwapChainSupportDetails VulkanRenderSystem::querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        // lower latency
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    // more energy efficient
    return VK_PRESENT_MODE_FIFO_KHR;
}


static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = {width, height};

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void VulkanRenderSystem::createSwapChain() {
    uint32_t width, height;
    mWindowHooks.getFramebufferSize(mWindowHooks.userData, width, height);
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(mPhysicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, width, height);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = mSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(mPhysicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mSwapChain);
    RUNTIME_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to create swap chain");

    vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCount, nullptr);
    mSwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCount, mSwapChainImages.data());

    mSwapChainImageFormat = surfaceFormat.format;
    mSwapChainExtent = extent;
}

void VulkanRenderSystem::createImageViews() {
    mSwapChainImageViews.resize(mSwapChainImages.size());

    for (size_t i = 0; i < mSwapChainImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = mSwapChainImages[i];
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        
        VkResult result = vkCreateImageView(mDevice, &createInfo, nullptr, &mSwapChainImageViews[i]);
        RUNTIME_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to create image view");
    }
}

void VulkanRenderSystem::createDescriptorPool() {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = mUniforms.size() * static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        
    VkResult result = vkCreateDescriptorPool(mDevice, &poolInfo, nullptr, &mDescriptorPool);
    RUNTIME_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to create descriptor pool");
}

void VulkanRenderSystem::createDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, mDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = mDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    VkResult result = vkAllocateDescriptorSets(mDevice, &allocInfo, mSceneDescriptorSets.data());
    RUNTIME_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to create scene descriptor sets");
}

void VulkanRenderSystem::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = mSwapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // this subpass dependency is needed because vulkan may do an implicit layout transition for the input before a swap chain image is acquired
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkResult result = vkCreateRenderPass(mDevice, &renderPassInfo, nullptr, &mRenderPass);
    RUNTIME_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to create render pass");
}

void VulkanRenderSystem::createFramebuffers() {
    mSwapChainFramebuffers.resize(mSwapChainImageViews.size());

    for (size_t i = 0; i < mSwapChainImageViews.size(); i++) {
        VkImageView attachments[] = {
            mSwapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = mRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = mSwapChainExtent.width;
        framebufferInfo.height = mSwapChainExtent.height;
        framebufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(mDevice, &framebufferInfo, nullptr, &mSwapChainFramebuffers[i]);
        RUNTIME_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to create framebuffer");
    }
}

void VulkanRenderSystem::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(mPhysicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    // not sure if this flag is needed
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    VkResult result = vkCreateCommandPool(mDevice, &poolInfo, nullptr, &mCommandPool);
    RUNTIME_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to create command pool");
}

void VulkanRenderSystem::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult imRes = vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]);
        RUNTIME_ASSERT(imRes == VK_SUCCESS, "Vulkan: Failed to create image wait semaphore");
        VkResult reRes = vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]);
        RUNTIME_ASSERT(reRes == VK_SUCCESS, "Vulkan: Failed to create renderFinished semaphore");
        VkResult feRes = vkCreateFence(mDevice, &fenceInfo, nullptr, &mInFlightFences[i]);
        RUNTIME_ASSERT(feRes == VK_SUCCESS, "Vulkan: Failed to create in flight fence");
    }
}

void VulkanRenderSystem::createCommandBuffer() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = mCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = mCommandBuffers.size();
    VkResult result = vkAllocateCommandBuffers(mDevice, &allocInfo, mCommandBuffers.data());
    RUNTIME_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to create command buffers");
}

void VulkanRenderSystem::createPushConstantBuffer() {
    size_t totalPushConstantSize = 0;
    size_t pushConstantMaxSize = 0;
    VkShaderStageFlags pushConstantAllStageFlags = 0;
    for (auto& d : mDrawables) {
        // calculate push constant needs
        int drawablePushConstantSize = 0;
        const csp::ProgramDescriptor& pd = d.drawable->programDescriptor();
        for (uint32_t i = 0; i < pd.push_constant_count; i++) {
            const csp::PushConstantEntry& pc = pd.push_constants[i];
            if (pc.offset + pc.size > drawablePushConstantSize) {
              drawablePushConstantSize = pc.offset + pc.size;
            }
            pushConstantAllStageFlags |= pc.stage_flags;
        }

        if (pushConstantMaxSize < drawablePushConstantSize) {
            pushConstantMaxSize = drawablePushConstantSize;
        }
        totalPushConstantSize += drawablePushConstantSize;
    }

    mPushConstantRange.stageFlags = pushConstantAllStageFlags;
    mPushConstantRange.offset = 0;
    mPushConstantRange.size = pushConstantMaxSize;
    mPushConstantBuffer = realloc(mPushConstantBuffer, totalPushConstantSize);

    size_t drawablePushConstantOffset = 0;
    for (auto& d : mDrawables) {
        // set push constant pointer
        uint32_t drawablePushConstantSize = d.drawable->pushConstantSize();
        d.drawable->setPushConstantData((uint8_t*)mPushConstantBuffer + drawablePushConstantOffset);
        drawablePushConstantOffset += drawablePushConstantSize;
    }
}

void VulkanRenderSystem::createSceneDescriptorSetLayout() {
    std::vector<VkDescriptorSetLayoutBinding> uboLayoutBindings(mUniforms.size());
    for (int i = 0; i < (int)mUniforms.size(); i++) {
        const VulkanUniformBuffer* uniform = static_cast<VulkanUniformBuffer*>(mUniforms[i]);
        uboLayoutBindings[i].binding = uniform->bindPoint();
        uboLayoutBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBindings[i].descriptorCount = 1;
        uboLayoutBindings[i].stageFlags = uniform->stageFlags();
    }

    // create descriptor set layout
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = uboLayoutBindings.size();
    layoutInfo.pBindings = uboLayoutBindings.data();
    VkResult setLayoutCreateResult = vkCreateDescriptorSetLayout(mDevice, &layoutInfo, nullptr, &mDescriptorSetLayout);
    RUNTIME_ASSERT(setLayoutCreateResult == VK_SUCCESS, "Vulkan: Failed to create scene descriptor set layout");

    // create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &mDescriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1; // Optional
    pipelineLayoutInfo.pPushConstantRanges = &mPushConstantRange; // Optional
    VkResult layoutCreateResult = vkCreatePipelineLayout(mDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout);
    RUNTIME_ASSERT(layoutCreateResult == VK_SUCCESS, "Vulkan: Failed to create pipeline layout");
}

void VulkanRenderSystem::recreateSwapChain() {
    // on minimization blocks until resurfaced again
    uint32_t width = 0, height = 0;
    mWindowHooks.getFramebufferSize(mWindowHooks.userData, width, height);
    while (width == 0 || height == 0) {
        mWindowHooks.getFramebufferSize(mWindowHooks.userData, width, height);
    }

    vkDeviceWaitIdle(mDevice);

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createFramebuffers();
}

void VulkanRenderSystem::cleanupSwapChain() {
    for (auto framebuffer : mSwapChainFramebuffers) {
        vkDestroyFramebuffer(mDevice, framebuffer, nullptr);
    }
    for (auto imageView : mSwapChainImageViews) {
        vkDestroyImageView(mDevice, imageView, nullptr);
    }
    vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);
}

void VulkanRenderSystem::recordCommandBuffer(VkCommandBuffer commandBuffer, const VkDescriptorSet& sceneDescriptorSet, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional
    VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    RUNTIME_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to begin recording command buffer");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = mRenderPass;
    renderPassInfo.framebuffer = mSwapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = mSwapChainExtent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &mClearColor;
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // set scizzor and viewport
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(mSwapChainExtent.width);
    viewport.height = static_cast<float>(mSwapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = mSwapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // bind descriptor set
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &sceneDescriptorSet, 0, nullptr);

    // draw render list
    mDrawList.recordCommands(commandBuffer, mPipelineLayout);

    // end drawing
    vkCmdEndRenderPass(commandBuffer);
    VkResult endCommandRes = vkEndCommandBuffer(commandBuffer);
    RUNTIME_ASSERT(endCommandRes == VK_SUCCESS, "Vulkan: Failed to end command buffer");
}

uint32_t VulkanRenderSystem::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    PANIC("Vulkan: Failed to find suitable memory type");
}

void VulkanRenderSystem::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkResult createRes = vkCreateBuffer(mDevice, &bufferInfo, nullptr, &buffer);
    RUNTIME_ASSERT(createRes == VK_SUCCESS, "Vulkan: Failed to create buffer");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(mDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
    VkResult allocRes = vkAllocateMemory(mDevice, &allocInfo, nullptr, &bufferMemory);
    RUNTIME_ASSERT(allocRes == VK_SUCCESS, "Vulkan: Failed to allocate buffer memory");

    vkBindBufferMemory(mDevice, buffer, bufferMemory, 0);
}

void VulkanRenderSystem::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = mCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(mDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(mGraphicsQueue);
    vkFreeCommandBuffers(mDevice, mCommandPool, 1, &commandBuffer);
}

void VulkanRenderSystem::setClearColor(float r, float g, float b, float a) {
    mClearColor = {{{r, g, b, a}}};
}

void VulkanRenderSystem::setViewport(int x, int y, int width, int height) {
    // TODO: non-full screen rendering
    mFramebufferResized = true;
}

void VulkanRenderSystem::addDrawable(Drawable* drawable) {
    VulkanDrawable d(this, drawable);
    mDrawables.push_back(d);
    mDrawList.addDrawable(&mDrawables.back());
}

RenderUniformBuffer* VulkanRenderSystem::addUniform(size_t size, uint32_t bindPoint) {
    RenderUniformBuffer* uni = new VulkanUniformBuffer(this, size, bindPoint);
    mUniforms.emplace_back(uni);
    return uni;
}

void VulkanRenderSystem::load() {
    createPushConstantBuffer();
    // Create pipeline layout and scene descriptor set layout
    createSceneDescriptorSetLayout();
    createDescriptorPool();
    createDescriptorSets();
    for (int i = 0; i < (int)mUniforms.size(); i++) {
        VulkanUniformBuffer* uniform = static_cast<VulkanUniformBuffer*>(mUniforms[i]);
        uniform->createBuffers(MAX_FRAMES_IN_FLIGHT);
    }

    for (auto& d : mDrawables) {
        // load program
        registerProgram(d.drawable);

        // create and load buffers, textures, uniforms etc
        d.load();
    }
}

void VulkanRenderSystem::renderFrame() {
    vkWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);

    // acquire image from swap chain
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(mDevice, mSwapChain, UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mFramebufferResized) {
        mFramebufferResized = false;
        recreateSwapChain();
        return;
    } else if (result != VK_SUCCESS) {
        PANIC("Vulkan: Failed to acquire swap chain image");
    }

    vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);

    // update current frame's descriptor set
    for (int i =0; i < (int)mUniforms.size(); i++) {
        VulkanUniformBuffer* uniform = static_cast<VulkanUniformBuffer*>(mUniforms[i]);
        uniform->update(mCurrentFrame);
    }

    // record command buffer (it is always necessary? What if the draw calls are not chainging? What if only uniforms are changing?)
    vkResetCommandBuffer(mCommandBuffers[mCurrentFrame], 0);
    recordCommandBuffer(mCommandBuffers[mCurrentFrame], mSceneDescriptorSets[mCurrentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = {mImageAvailableSemaphores[mCurrentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommandBuffers[mCurrentFrame];
    VkSemaphore signalSemaphores[] = {mRenderFinishedSemaphores[mCurrentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    VkResult drawResult = vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[mCurrentFrame]);
    RUNTIME_ASSERT(drawResult == VK_SUCCESS, "Vulkan: Failed to submit draw command buffer to queue");

    // presentation
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = {mSwapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional
    vkQueuePresentKHR(mPresentQueue, &presentInfo);

    mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanRenderSystem::registerProgram(Drawable* drawable) {
    VulkanPipelineSignature sig;
    sig.programDescriptor = drawable->programDescriptor();
    sig.primitiveType = drawable->primitiveType();
    auto it = mPrograms.find(sig);

    if (it != mPrograms.end()) {
        drawable->setProgram(it->second);
    } else {
        RenderProgram* program = new VulkanProgram(mDevice, mRenderPass, mPipelineLayout, drawable);
        mPrograms[sig] = program;
        drawable->setProgram(program);
    }
}

} // gfx
} // bolt