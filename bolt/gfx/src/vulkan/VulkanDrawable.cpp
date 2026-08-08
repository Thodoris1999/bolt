#include "gfx/vulkan/VulkanDrawable.hpp"
#include "csp/csp.hpp"
#include "gfx/vulkan/VulkanRenderSystem.hpp"
#include "util/common.h"

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

void VulkanDrawable::createUniformBuffers() {
    const VulkanProgram& program = mRenderSystem->getProgram(DrawKey::getPipeline(drawKey));
    VkDescriptorSetLayout uniformSetLayout = program.uniformDescriptorSetLayout();
    int numFramesInFlight = mRenderSystem->maxFramesInFlight();

    const csp::ProgramDescriptor& pd = drawable->programDescriptor();
    const csp::UniformVar* uniformVars = pd.uniform_vars;
    for (uint32_t i = 0; i < pd.uniform_count; i++) {
        const csp::UniformVar& uv = uniformVars[i];
        if (uv.set != 2) {
            continue;
        }
        if (uv.size == 0) {
            PANIC("Vulkan: set=2 uniform block has zero size for \"%.*s\" — was csp regenerated with UniformVar.size support?", (int)uv.name.size(), uv.name.data());
        }

        auto uniform = std::make_unique<VulkanUniformBuffer>(mRenderSystem, uv.size, uv.binding);
        uniform->createBuffers(numFramesInFlight);
        drawable->setUniformData(i, uniform.get());
        mUniformBuffers.push_back(std::move(uniform));
    }

    if (mUniformBuffers.empty()) {
        return;
    }

    std::vector<VkDescriptorSetLayout> setLayouts(numFramesInFlight, uniformSetLayout);
    mUniformDescriptorSets.resize(numFramesInFlight);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = mRenderSystem->descriptorPool();
    allocInfo.descriptorSetCount = numFramesInFlight;
    allocInfo.pSetLayouts = setLayouts.data();
    VkResult result = vkAllocateDescriptorSets(mRenderSystem->device(), &allocInfo, mUniformDescriptorSets.data());
    RUNTIME_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to allocate drawable uniform descriptor sets");

    for (int f = 0; f < numFramesInFlight; f++) {
        for (auto& uniform : mUniformBuffers) {
            uniform->writeDescriptorSet(mUniformDescriptorSets[f], f);
        }
    }
}

void VulkanDrawable::updateUniformBuffers(uint32_t frameIndex) {
    for (auto& uniform : mUniformBuffers) {
        uniform->update(frameIndex);
    }
}

void VulkanDrawable::load() {
    createVertexBuffer();
    mHasIndexBuffer = drawable->indexCount() > 0;
    if (mHasIndexBuffer) {
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

    createUniformBuffers();

    mLoaded = true;
}

void VulkanDrawable::unload() {
    if (mHasIndexBuffer) {
        vkDestroyBuffer(mRenderSystem->device(), mIndexBuffer, nullptr);
        vkFreeMemory(mRenderSystem->device(), mIndexBufferMemory, nullptr);
    }

    vkDestroyBuffer(mRenderSystem->device(), mVertexBuffer, nullptr);
    vkFreeMemory(mRenderSystem->device(), mVertexBufferMemory, nullptr);

    free(mPushConstantBuffer);

    mUniformBuffers.clear();

    mLoaded = false;
}

} // gfx
} // bolt