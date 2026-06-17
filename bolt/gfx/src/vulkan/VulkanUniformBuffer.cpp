#include "gfx/vulkan/VulkanUniformBuffer.hpp"
#include "gfx/vulkan/VulkanRenderSystem.hpp"

#include <cstring>

namespace bolt {
namespace gfx {

VulkanUniformBuffer::VulkanUniformBuffer(const VulkanRenderSystem* renderSystem, size_t size, uint32_t bindPoint)
        : mRenderSystem(renderSystem), mSize(size), mBindPoint(bindPoint) {
    // TODO: support specifying shader flags
    mStageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    mStagingBuffer = malloc(size);
}

VulkanUniformBuffer::~VulkanUniformBuffer() {
    free(mStagingBuffer);
    for (size_t i = 0; i < mUniformBuffers.size(); i++) {
        vkDestroyBuffer(mRenderSystem->device(), mUniformBuffers[i], nullptr);
        vkFreeMemory(mRenderSystem->device(), mUniformBuffersMemory[i], nullptr);
    }
}

void VulkanUniformBuffer::writeData(const void* src, size_t offset, size_t size) {
    memcpy((uint8_t*)mStagingBuffer + offset, src, size);
}

void VulkanUniformBuffer::createBuffers(int numFrames) {
    mUniformBuffers.resize(numFrames);
    mUniformBuffersMemory.resize(numFrames);
    mUniformBuffersMapped.resize(numFrames);

    for (int i = 0; i < numFrames; i++) {
        mRenderSystem->createBuffer(mSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            mUniformBuffers[i], mUniformBuffersMemory[i]);

        vkMapMemory(mRenderSystem->device(), mUniformBuffersMemory[i], 0, mSize, 0, &mUniformBuffersMapped[i]);

        // update descriptor set
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = mUniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = mSize;

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.pNext = nullptr;
        descriptorWrite.dstSet = mRenderSystem->sceneDescriptorSet(i);
        descriptorWrite.dstBinding = mBindPoint;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;
        vkUpdateDescriptorSets(mRenderSystem->device(), 1, &descriptorWrite, 0, nullptr);
    }
}

void VulkanUniformBuffer::update(int currentImage) {
    memcpy(mUniformBuffersMapped[currentImage], mStagingBuffer, mSize);
}

} // gfx
} // bolt
