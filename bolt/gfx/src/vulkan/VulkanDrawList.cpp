#include "gfx/vulkan/VulkanDrawList.hpp"
#include "gfx/vulkan/VulkanDrawable.hpp"
#include "gfx/vulkan/VulkanMaterial.hpp"
#include "gfx/vulkan/VulkanProgram.hpp"
#include "gfx/vulkan/VulkanRenderSystem.hpp"

#include <algorithm>

namespace bolt {
namespace gfx {

void VulkanDrawList::addDrawable(VulkanDrawable* drawable) {
    mDrawables.push_back(drawable);
}

void VulkanDrawList::removeDrawable(VulkanDrawable* drawable) {
    auto it = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    if (it == mDrawables.end()) return;
    mDrawables.erase(it);
}

void VulkanDrawList::recordCommands(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet, uint32_t frameIndex) {
    std::sort(mDrawables.begin(), mDrawables.end(), [](const VulkanDrawable* a, const VulkanDrawable* b) {
        return a->drawKey < b->drawKey;
    });

    uint32_t currentPipelineId = -1;
    uint32_t currentMaterialId = -1;

    for (const auto& drawable : mDrawables) {
        if (!drawable->drawable->visible()) continue;

        uint32_t pipelineId = DrawKey::getPipeline(drawable->drawKey);
        if (currentPipelineId != pipelineId) {
            const VulkanProgram& program = mRenderSystem->getProgram(pipelineId);
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program.pipeline());
            // rebind scene descriptor set
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program.pipelineLayout(), 0, 1, &sceneDescriptorSet, 0, nullptr);
            currentPipelineId = pipelineId;
        }
        const VulkanProgram& program = mRenderSystem->getProgram(currentPipelineId);
        
        uint32_t materialId = DrawKey::getMaterial(drawable->drawKey);
        if (currentMaterialId != materialId) {
            const VulkanMaterial& material = mRenderSystem->getMaterial(materialId);
            VkDescriptorSet dSet = material.descriptorSet();
            if (dSet != VK_NULL_HANDLE) {
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program.pipelineLayout(), 1, 1, &dSet, 0, nullptr);
            }
            currentMaterialId = materialId;
        }

        Drawable* d = drawable->drawable;
        // bind vertex buffer
        VkBuffer vertexBuffers[] = {drawable->vertexBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        // pre-draw operations
        d->onDraw();

        // flush this drawable's set=2 uniform blocks for the current
        // frame-in-flight, and bind them if present
        drawable->updateUniformBuffers(frameIndex);
        VkDescriptorSet uniformSet = drawable->uniformDescriptorSet(frameIndex);
        if (uniformSet != VK_NULL_HANDLE) {
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program.pipelineLayout(), 2, 1, &uniformSet, 0, nullptr);
        }

        // push constants
        const csp::ProgramDescriptor& pd = d->programDescriptor();
        for (uint32_t i = 0; i < pd.push_constant_range_count; i++) {
            const csp::PushConstantRange& pc = pd.push_constant_ranges[i];
            vkCmdPushConstants(commandBuffer, program.pipelineLayout(), pc.stage_flags, pc.offset, pc.size, static_cast<uint8_t*>(d->pushConstantData()) + pc.offset);
        }

        // draw cmd
        if (d->drawOp() == BOLT_GFX_ARRAY) {
            vkCmdDraw(commandBuffer, d->vertexCount(), 1, 0, 0);
        } else {
            vkCmdBindIndexBuffer(commandBuffer, drawable->indexBuffer(), 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(commandBuffer, d->indexCount(), 1, 0, 0, 0);
        }
    }
}

} //gfx
} // bolt