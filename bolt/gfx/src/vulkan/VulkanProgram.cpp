#include "gfx/vulkan/VulkanProgram.hpp"
#include "gfx/vulkan/VulkanRenderSystem.hpp"
#include "csp/csp.hpp"
#include "util/Filesystem.hpp"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace bolt {
namespace gfx {

inline VkFormat toVkFormat(DataType type, int count) {
    switch (type) {
    case BOLT_F32:
        switch (count) {
        case 1: return VK_FORMAT_R32_SFLOAT;
        case 2: return VK_FORMAT_R32G32_SFLOAT;
        case 3: return VK_FORMAT_R32G32B32_SFLOAT;
        case 4: return VK_FORMAT_R32G32B32A32_SFLOAT;
        default:
            PANIC("Unsupported float count %d", count);
            return VK_FORMAT_UNDEFINED;
        }
    default:
        PANIC("Unsupported DataType %d", static_cast<int>(type));
        return VK_FORMAT_UNDEFINED;
    }
}

struct VertexInputState {
    VkPipelineVertexInputStateCreateInfo info{};
    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;
};

inline VertexInputState createVertexInputState(const Drawable* drawable) {
    VertexInputState state{};

    const VertexAttribute* attrs = drawable->attributes();
    const int attrCount = drawable->attributeCount();

    // --- Binding (single interleaved buffer) ---
    VkVertexInputBindingDescription binding{};
    binding.binding   = 0;
    binding.stride    = drawable->vertexSize();
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    state.bindings.push_back(binding);

    // --- Attributes ---
    for (int i = 0; i < attrCount; ++i) {
        const VertexAttribute& a = attrs[i];

        VkVertexInputAttributeDescription attr{};
        attr.location = a.index;          // shader location
        attr.binding  = 0;
        attr.format   = toVkFormat(a.dtype, a.count);
        attr.offset   = a.offset;

        state.attributes.push_back(attr);
    }

    // --- Pipeline state ---
    state.info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    state.info.vertexBindingDescriptionCount =
        static_cast<uint32_t>(state.bindings.size());
    state.info.pVertexBindingDescriptions = state.bindings.data();
    state.info.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(state.attributes.size());
    state.info.pVertexAttributeDescriptions = state.attributes.data();

    return state;
}

static VkShaderModule createShaderModule(VkDevice device, const std::string_view& filename) {
    unsigned int size;
    void* source = util::Filesystem::loadFile(filename, size);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = size;
    createInfo.pCode = reinterpret_cast<const uint32_t*>(source);

    VkShaderModule shaderModule;
    VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
    RUNTIME_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to load module");
    return shaderModule;
}

static VkShaderStageFlagBits csp2vkStageFlag(csp::ShaderStage stage) {
    switch (stage) {
    case csp::ShaderStage::Vertex:
        return VK_SHADER_STAGE_VERTEX_BIT;
    
    case csp::ShaderStage::TessControl:
        return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    
    case csp::ShaderStage::TessEval:
        return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    
    case csp::ShaderStage::Geometry:
        return VK_SHADER_STAGE_GEOMETRY_BIT;
    
    case csp::ShaderStage::Fragment:
        return VK_SHADER_STAGE_FRAGMENT_BIT;
    
    case csp::ShaderStage::Compute:
        return VK_SHADER_STAGE_COMPUTE_BIT;
    
    default:
        PANIC("Vulkan: Unkown shader stage");
        return VK_SHADER_STAGE_COMPUTE_BIT;
    }
}

static VkDescriptorType csp2vkDescriptorType(csp::DescriptorType type) {
    switch (type) {
    case csp::DescriptorType::Sampler:
        return VK_DESCRIPTOR_TYPE_SAMPLER;
    
    case csp::DescriptorType::CombinedImageSampler:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    
    case csp::DescriptorType::SampledImage:
        return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    
    case csp::DescriptorType::StorageImage:
        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    
    case csp::DescriptorType::UniformTexelBuffer:
        return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    
    case csp::DescriptorType::StorageTexelBuffer:
        return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    
    case csp::DescriptorType::UniformBuffer:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    
    default:
        PANIC("Vulkan: Unkown descriptor type");
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
}

VulkanProgram::VulkanProgram(const VulkanRenderSystem* renderSystem, Drawable* drawable) : RenderProgram(&drawable->programDescriptor()), mRenderSystem(renderSystem) {
    // create descriptor layout (set=1) and pipeline layout
    // descriptor bindings
    mDescriptorSetLayout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSetLayoutBinding> descriptorBindings;
    const csp::ProgramDescriptor& pDesc = drawable->programDescriptor();
    const csp::UniformVar* uniformVars = pDesc.uniform_vars;
    for (uint32_t i = 0; i < pDesc.uniform_count; i++) {
        const csp::UniformVar& uniformVar = uniformVars[i];
        if (uniformVar.set == 1) {
            VkDescriptorSetLayoutBinding binding{};
            binding.binding = uniformVar.binding;
            binding.descriptorType = csp2vkDescriptorType(uniformVar.descriptor_type);
            binding.descriptorCount = 1;
            binding.stageFlags = uniformVar.stage_flags;
            descriptorBindings.push_back(binding);
        }
    }

    // descriptor layout
    if (descriptorBindings.size() > 0) {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = descriptorBindings.size();
        layoutInfo.pBindings = descriptorBindings.data();
        VkResult setLayoutCreateResult = vkCreateDescriptorSetLayout(renderSystem->device(), &layoutInfo, nullptr, &mDescriptorSetLayout);
        RUNTIME_ASSERT(setLayoutCreateResult == VK_SUCCESS, "Vulkan: Failed to create pipeline descriptor set layout");
    }

    // push constant ranges
    std::vector<VkPushConstantRange> pcRanges(pDesc.push_constant_count);
    for (uint32_t i = 0; i < pDesc.push_constant_count; i++) {
        const csp::PushConstantEntry& pushConstant = pDesc.push_constants[i];
        pcRanges[i].stageFlags = pushConstant.stage_flags;
        pcRanges[i].offset = pushConstant.offset;
        pcRanges[i].size = pushConstant.size;
    }

    // pipeline layout
    std::vector<VkDescriptorSetLayout> layouts = {mDescriptorSetLayout};
    if (descriptorBindings.size() > 0) {
        layouts = {mRenderSystem->sceneDescriptorSetLayout(), mDescriptorSetLayout};
    } else {
        layouts = {mRenderSystem->sceneDescriptorSetLayout()};
    }
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = layouts.size();
    pipelineLayoutInfo.pSetLayouts = layouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = pcRanges.size();
    pipelineLayoutInfo.pPushConstantRanges = pcRanges.data();
    VkResult layoutCreateResult = vkCreatePipelineLayout(renderSystem->device(), &pipelineLayoutInfo, nullptr, &mPipelineLayout);
    RUNTIME_ASSERT(layoutCreateResult == VK_SUCCESS, "Vulkan: Failed to create pipeline layout");

    // create pipeline
    const csp::ProgramDescriptor& pd = drawable->programDescriptor();
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    shaderStages.reserve(pd.source_count);

    std::vector<VkShaderModule> shaderModules;
    shaderModules.reserve(pd.source_count);

    for (uint32_t i = 0; i < pd.source_count; ++i) {
        const auto& src = pd.vk_sources[i];

        // Create shader module from filename
        VkShaderModule module = createShaderModule(renderSystem->device(), src.filename);
        shaderModules.push_back(module);

        VkPipelineShaderStageCreateInfo stageInfo{};
        stageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.stage  = csp2vkStageFlag(src.stage);
        stageInfo.module = module;
        stageInfo.pName  = "main";

        shaderStages.push_back(stageInfo);
    }

    // vertex input description
    VertexInputState vertexInput = createVertexInputState(drawable);

    // input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    switch (drawable->primitiveType())
    {
    case BOLT_GFX_TRIANGLE:
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        break;
    
    // TODO: Remove triangle fan support for portability reasons
    case BOLT_GFX_TRIANGLE_FAN:
        PANIC("Vulkan: BOLT_GFX_TRIANGLE_FAN topology not supported by vulkan backend");
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        break;
    
    case BOLT_GFX_LINE:
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        break;
    
    default:
        PANIC("Vulkan: Unknown primitive type %d", drawable->primitiveType());
        break;
    }
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // dynamic viewport and scizzor
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    // rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // VK_POLYGON_MODE_LINE for wireframe
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    // disabled multisampling
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    // disable blending for now
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();

    pipelineInfo.pVertexInputState = &vertexInput.info;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;

    pipelineInfo.layout = mPipelineLayout;

    pipelineInfo.renderPass = mRenderSystem->renderPass();
    pipelineInfo.subpass = 0;

    // TODO: consider inheriting (e.g. for much of common fixed pipeline info)
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    VkResult result = vkCreateGraphicsPipelines(mRenderSystem->device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mGraphicsPipeline);
    RUNTIME_ASSERT(result == VK_SUCCESS, "Vulkan: Failed to create pipeline");

    for (auto module : shaderModules) {
        vkDestroyShaderModule(mRenderSystem->device(), module, nullptr);
    }
}

VulkanProgram::~VulkanProgram() {
    vkDestroyDescriptorSetLayout(mRenderSystem->device(), mDescriptorSetLayout, nullptr);
    vkDestroyPipelineLayout(mRenderSystem->device(), mPipelineLayout, nullptr);
    vkDestroyPipeline(mRenderSystem->device(), mGraphicsPipeline, nullptr);
}

} // gfx
} // bolt