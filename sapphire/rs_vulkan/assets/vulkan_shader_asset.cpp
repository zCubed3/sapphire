#include "vulkan_shader_asset.h"

#include <rs_vulkan/rendering/vulkan_render_server.h>
#include <rs_vulkan/rendering/vulkan_mesh_buffer.h>
#include <rs_vulkan/val/val_instance.h>

bool VulkanShaderAsset::create_module(const std::vector<char> &code, VkShaderModule *p_module) {
    VkShaderModuleCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    const VulkanRenderServer* render_server = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());
    ValInstance* val_instance = render_server->val_instance;

    return vkCreateShaderModule(val_instance->vk_device, &create_info, nullptr, p_module) == VK_SUCCESS;
}

VkPipelineShaderStageCreateInfo VulkanShaderAsset::create_stage(Stage stage, VkShaderModule p_module) {
    VkShaderStageFlagBits stage_flags;

    switch (stage) {
        case Stage::Vertex:
            stage_flags = VK_SHADER_STAGE_VERTEX_BIT;
            break;

        case Stage::Fragment:
            stage_flags = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
    }

    VkPipelineShaderStageCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    create_info.stage = stage_flags;
    create_info.module = p_module;
    create_info.pName = "main"; // TODO: Custom entry points?

    return create_info;
}

VkVertexInputBindingDescription VulkanShaderAsset::get_input_binding() {
    VkVertexInputBindingDescription binding_description{};
    binding_description.binding = 0;
    binding_description.stride = sizeof(MeshBuffer::Vertex);
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return binding_description;
}

std::vector<VkVertexInputAttributeDescription> VulkanShaderAsset::get_input_attributes() {
    std::vector<VkVertexInputAttributeDescription> input_attributes(3);

    input_attributes[0].binding = 0;
    input_attributes[0].location = 0;
    input_attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    input_attributes[0].offset = 0;

    input_attributes[1].binding = 0;
    input_attributes[1].location = 1;
    input_attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    input_attributes[1].offset = sizeof(glm::vec3);

    input_attributes[2].binding = 0;
    input_attributes[2].location = 2;
    input_attributes[2].format = VK_FORMAT_R32G32_SFLOAT;
    input_attributes[2].offset = sizeof(glm::vec3) * 2;

    return input_attributes;
}

std::vector<VkDynamicState> VulkanShaderAsset::get_dynamic_states(uint32_t flags) {
    std::vector<VkDynamicState> states {};

    if (flags & DynamicStateFlags::Viewport) {
        states.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    }

    if (flags & DynamicStateFlags::Scissor) {
        states.push_back(VK_DYNAMIC_STATE_SCISSOR);
    }

    return states;
}

void VulkanShaderAsset::create_vert_frag(const std::vector<char> &vert_code, const std::vector<char> &frag_code)  {
    VkShaderModule vert_module;
    VkShaderModule frag_module;

    create_module(vert_code, &vert_module);
    create_module(frag_code, &frag_module);

    VkPipelineShaderStageCreateInfo vert_stage = create_stage(Stage::Vertex, vert_module);
    VkPipelineShaderStageCreateInfo frag_stage = create_stage(Stage::Fragment, frag_module);

    const VulkanRenderServer* render_server = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());
    ValInstance* val_instance = render_server->val_instance;

    const std::vector<VkDynamicState> dynamic_states = get_dynamic_states(dynamic_flags);

    VkPipelineDynamicStateCreateInfo dynamic_state_create_info{};
    dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    dynamic_state_create_info.pDynamicStates = dynamic_states.data();

    VkVertexInputBindingDescription input_description = get_input_binding();
    std::vector<VkVertexInputAttributeDescription> input_attributes = get_input_attributes();

    VkPipelineVertexInputStateCreateInfo vertex_input_create_info{};
    vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_create_info.vertexBindingDescriptionCount = 1;
    vertex_input_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(input_attributes.size());
    vertex_input_create_info.pVertexBindingDescriptions = &input_description;
    vertex_input_create_info.pVertexAttributeDescriptions = input_attributes.data();

    VkVertexInputBindingDescription bindingDescription{};

    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info{};
    input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewport_state_create_info{};
    viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_create_info.viewportCount = 1;
    viewport_state_create_info.scissorCount = 1;

    // TODO: Culling mode
    VkPipelineRasterizationStateCreateInfo rasterizer_create_info{};
    rasterizer_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer_create_info.depthClampEnable = VK_FALSE;
    rasterizer_create_info.rasterizerDiscardEnable = VK_FALSE;
    rasterizer_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer_create_info.lineWidth = 1.0f;
    rasterizer_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer_create_info.depthBiasEnable = VK_FALSE;
    rasterizer_create_info.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer_create_info.depthBiasClamp = 0.0f; // Optional
    rasterizer_create_info.depthBiasSlopeFactor = 0.0f; // Optional

    // TODO: MSAA
    VkPipelineMultisampleStateCreateInfo multisampling_create_info{};
    multisampling_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_create_info.sampleShadingEnable = VK_FALSE;
    multisampling_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling_create_info.minSampleShading = 1.0f; // Optional
    multisampling_create_info.pSampleMask = nullptr; // Optional
    multisampling_create_info.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling_create_info.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineColorBlendAttachmentState color_blend_attachment_state{};
    color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment_state.blendEnable = VK_FALSE;
    color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo color_blend_state{};
    color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state.logicOpEnable = VK_FALSE;
    color_blend_state.logicOp = VK_LOGIC_OP_COPY; // Optional
    color_blend_state.attachmentCount = 1;
    color_blend_state.pAttachments = &color_blend_attachment_state;
    color_blend_state.blendConstants[0] = 0.0f; // Optional
    color_blend_state.blendConstants[1] = 0.0f; // Optional
    color_blend_state.blendConstants[2] = 0.0f; // Optional
    color_blend_state.blendConstants[3] = 0.0f; // Optional

    VkPipelineDepthStencilStateCreateInfo depth_stencil_state{};
    depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_state.depthTestEnable = VK_TRUE;
    depth_stencil_state.depthWriteEnable = VK_TRUE;
    depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS;
    depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_state.minDepthBounds = 0.0f; // Optional
    depth_stencil_state.maxDepthBounds = 1.0f; // Optional
    depth_stencil_state.stencilTestEnable = VK_FALSE;
    depth_stencil_state.front = {}; // Optional
    depth_stencil_state.back = {}; // Optional

    VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.setLayoutCount = 1; // Optional
    pipeline_layout_create_info.pSetLayouts = &render_server->vk_descriptor_set_layout; // Optional
    pipeline_layout_create_info.pushConstantRangeCount = 0; // Optional
    pipeline_layout_create_info.pPushConstantRanges = nullptr; // Optional

    vkCreatePipelineLayout(val_instance->vk_device, &pipeline_layout_create_info, nullptr, &vk_pipeline_layout);

    VkPipelineShaderStageCreateInfo stages[] = {
            vert_stage,
            frag_stage
    };

    VkGraphicsPipelineCreateInfo pipeline_create_info{};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.stageCount = 2;
    pipeline_create_info.pStages = stages;
    pipeline_create_info.pVertexInputState = &vertex_input_create_info;
    pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
    pipeline_create_info.pViewportState = &viewport_state_create_info;
    pipeline_create_info.pRasterizationState = &rasterizer_create_info;
    pipeline_create_info.pMultisampleState = &multisampling_create_info;
    pipeline_create_info.pDepthStencilState = &depth_stencil_state; // Optional
    pipeline_create_info.pColorBlendState = &color_blend_state;
    pipeline_create_info.pDynamicState = &dynamic_state_create_info;
    pipeline_create_info.layout = vk_pipeline_layout;
    pipeline_create_info.renderPass = val_instance->vk_render_pass;
    pipeline_create_info.subpass = 0;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipeline_create_info.basePipelineIndex = -1; // Optional

    // TODO: Error handling
    // TODO: Pipeline caching
    vkCreateGraphicsPipelines(val_instance->vk_device,
                              nullptr,
                              1,
                              &pipeline_create_info,
                              nullptr,
                              &vk_pipeline);

    vkDestroyShaderModule(val_instance->vk_device, vert_module, nullptr);
    vkDestroyShaderModule(val_instance->vk_device, frag_module, nullptr);

    //vkDestroyDescriptorSetLayout(val_instance->vk_device, descriptor_set_layout, nullptr);
    //vkDestroyPipelineLayout(val_instance->vk_device, layout, nullptr);
}

VulkanShaderAsset::~VulkanShaderAsset() {
    const VulkanRenderServer* render_server = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());
    ValInstance* val_instance = render_server->val_instance;

    //vkDestroyDescriptorSetLayout(val_instance->vk_device, vk_descriptor_set_layout, nullptr);
    vkDestroyPipelineLayout(val_instance->vk_device, vk_pipeline_layout, nullptr);
    vkDestroyPipeline(val_instance->vk_device, vk_pipeline, nullptr);
}