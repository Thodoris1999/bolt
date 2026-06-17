#include "gfx/vulkan/VulkanDrawable.hpp"
#include "csp/csp.hpp"
#include "gfx/vulkan/VulkanRenderSystem.hpp"

#include <vulkan/vulkan.h>

#include <cstring>

namespace bolt {
namespace gfx {

void VulkanDrawable::createVertexBuffer() {
    VkDeviceSize bufferSize = drawable->vertexSize() * drawable->vertexCount();
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    mRenderSystem->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(mRenderSystem->device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, drawable->vertexData(), (size_t) bufferSize);
    vkUnmapMemory(mRenderSystem->device(), stagingBufferMemory);
    mRenderSystem->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        mVertexBuffer, mVertexBufferMemory);
    mRenderSystem->copyBuffer(stagingBuffer, mVertexBuffer, bufferSize);

    vkDestroyBuffer(mRenderSystem->device(), stagingBuffer, nullptr);
    vkFreeMemory(mRenderSystem->device(), stagingBufferMemory, nullptr);
}

void VulkanDrawable::createIndexBuffer() {
    VkDeviceSize bufferSize = drawable->indexCount() * sizeof(*drawable->indexData());

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    mRenderSystem->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(mRenderSystem->device(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, drawable->indexData(), (size_t) bufferSize);
    vkUnmapMemory(mRenderSystem->device(), stagingBufferMemory);

    mRenderSystem->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mIndexBuffer, mIndexBufferMemory);

    mRenderSystem->copyBuffer(stagingBuffer, mIndexBuffer, bufferSize);

    vkDestroyBuffer(mRenderSystem->device(), stagingBuffer, nullptr);
    vkFreeMemory(mRenderSystem->device(), stagingBufferMemory, nullptr);
}

void VulkanDrawable::load() {
    createVertexBuffer();
    if (drawable->indexCount() > 0) {
        createIndexBuffer();
    }

    // allocate push constant buffer so that it can hold any data in the push constant ranges
    uint32_t pcSize = 0;
    const csp::ProgramDescriptor& pd = drawable->programDescriptor();
    for (uint32_t i = 0; i < pd.push_constant_range_count; i++) {
        const csp::PushConstantRange& pc = pd.push_constant_ranges[i];
        if (pc.offset + pc.size > pcSize) {
            pcSize = pc.offset + pc.size;
        }
    }
    mPushConstantBuffer = malloc(pcSize);

    mLoaded = true;
}

void VulkanDrawable::unload() {
    if (drawable->indexCount() > 0) {
        vkDestroyBuffer(mRenderSystem->device(), mIndexBuffer, nullptr);
        vkFreeMemory(mRenderSystem->device(), mIndexBufferMemory, nullptr);
    }

    vkDestroyBuffer(mRenderSystem->device(), mVertexBuffer, nullptr);
    vkFreeMemory(mRenderSystem->device(), mVertexBufferMemory, nullptr);

    free(mPushConstantBuffer);

    mLoaded = false;
}

} // gfx
} // bolt