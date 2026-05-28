#include "gfx/vulkan/VulkanDrawList.hpp"
#include "gfx/vulkan/VulkanProgram.hpp"
#include <vulkan/vulkan_core.h>

namespace bolt {
namespace gfx {

void VulkanDrawList::addDrawable(VulkanDrawable* drawable) {
    VulkanPipelineSignature sig;
    sig.programDescriptor = drawable->drawable->programDescriptor();
    sig.primitiveType = drawable->drawable->primitiveType();

    mDrawables[sig].push_back(drawable);
}

void VulkanDrawList::recordCommands(VkCommandBuffer commandBuffer) {
    for (auto& [signature, drawables] : mDrawables) {
        // bind pipeline (same for all drawables in bucket)
        VulkanDrawable* first = drawables[0];
        VulkanProgram* program = static_cast<VulkanProgram*>(first->drawable->program());
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program->pipeline());

        // TODO: material pipeline level descriptor set (e.g. textures)

        for (VulkanDrawable* drawable : drawables) {
            Drawable* d = drawable->drawable;
            // bind vertex buffer
            VkBuffer vertexBuffers[] = {drawable->vertexBuffer()};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

            // pre-draw operations
            d->onDraw();

            // push constants
            const csp::ProgramDescriptor& pd = d->programDescriptor();
            for (uint32_t i = 0; i < pd.push_constant_count; i++) {
                const csp::PushConstantEntry& pc = pd.push_constants[i];
                vkCmdPushConstants(commandBuffer, program->pipelineLayout(), pc.stage_flags, pc.offset, pc.size, static_cast<uint8_t*>(d->pushConstantData()) + pc.offset);
            }

            // bind textures if needed

            // draw cmd
            if (d->drawOp() == BOLT_GFX_ARRAY) {
                vkCmdDraw(commandBuffer, d->vertexCount(), 1, 0, 0);
            } else {
                vkCmdBindIndexBuffer(commandBuffer, drawable->indexBuffer(), 0, VK_INDEX_TYPE_UINT16);
                vkCmdDrawIndexed(commandBuffer, d->indexCount(), 1, 0, 0, 0);
            }
        }
    }
}

} //gfx
} // bolt