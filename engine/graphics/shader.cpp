/*
MIT License

Copyright (c) 2023 zCubed (Liam R.)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "shader.hpp"

#include <engine.hpp>
#include <graphics/vulkan_provider.hpp>

#include <iostream>
#include <stdexcept>

using namespace Sapphire;

void Graphics::ShaderModule::compile(Graphics::VulkanProvider *p_provider) {
    if (p_provider == nullptr) {
        throw std::runtime_error("p_provider was nullptr!");
    }

    VkShaderModuleCreateInfo module_create_info {};
    module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    module_create_info.codeSize = data.size();
    module_create_info.pCode = reinterpret_cast<const uint32_t*>(data.data());

    VkResult result = vkCreateShaderModule(p_provider->get_vk_device(), &module_create_info, nullptr, &vk_module);

    if (result != VK_SUCCESS) {
        LOG_GRAPHICS("Error: vkCreateShaderModule failed with error code (" << result << ")");
        throw std::runtime_error("vkCreateShaderModule failed! Please check the log above for more info!");
    }

    VkShaderStageFlagBits stage_flags;

    switch (module_type) {
        case ModuleType::Vertex:
            stage_flags = VK_SHADER_STAGE_VERTEX_BIT;
            break;

        case ModuleType::Fragment:
            stage_flags = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
    }

    vk_stage_info = {};
    vk_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vk_stage_info.stage = stage_flags;
    vk_stage_info.module = vk_module;
    vk_stage_info.pName = entry_point.c_str();
}

// TODO: Deferred compiles?
Graphics::ShaderModule::ShaderModule(ModuleType type, std::vector<char> data, std::string entry_point) {
    this->module_type = type;
    this->data = data;
    this->entry_point = entry_point;
}

Graphics::ShaderModule::ShaderModule(Graphics::VulkanProvider *p_provider, ModuleType type, std::vector<char> data, std::string entry_point)
    : ShaderModule(type, data, entry_point)
{
    compile(p_provider);
}

VkPipelineShaderStageCreateInfo Graphics::ShaderModule::get_vk_stage_info() {
    return vk_stage_info;
}

std::function<void(Graphics::VulkanProvider*)> Graphics::Shader::get_release_func() {
    return [](VulkanProvider* p_provider){

    };
}

void Graphics::Shader::compile(VulkanProvider *p_provider, ShaderProperties properties, const std::vector<std::shared_ptr<ShaderModule>>& shader_modules) {
    if (p_provider == nullptr) {
        throw std::runtime_error("p_provider was nullptr!");
    }

    //
    // Our inputs
    //
    std::vector<VkPipelineShaderStageCreateInfo> vk_stage_infos;
    VkVertexInputBindingDescription vk_vtx_binding = p_provider->get_vk_vtx_binding();
    std::vector<VkVertexInputAttributeDescription> vk_vtx_attributes = p_provider->get_vk_vtx_attributes();

    // TODO: More dynamic states / changing this per platform (e.g. mobile)?
    const std::vector<VkDynamicState> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };

    for (const auto& sm : shader_modules) {
        if (sm == nullptr) {
            throw std::runtime_error("A provided shader module was nullptr");
        }

        vk_stage_infos.push_back(sm->get_vk_stage_info());
    }

    //
    // ShaderProperties to Vulkan types
    //
    VkCullModeFlagBits cull_flags;
    VkPolygonMode polygon_mode;
    VkFrontFace front_face;
    VkCompareOp compare_op;

    switch (properties.cull_mode) {
        case CullMode::Off:
            cull_flags = VK_CULL_MODE_NONE;
            break;

        case CullMode::Back:
            cull_flags = VK_CULL_MODE_BACK_BIT;
            break;

        case CullMode::Front:
            cull_flags = VK_CULL_MODE_FRONT_BIT;
            break;
    }

    // TODO: Check if the device allows non-solid fill modes!
    switch (properties.fill_mode) {
        case FillMode::Face:
            polygon_mode = VK_POLYGON_MODE_FILL;
            break;

        case FillMode::Line:
            polygon_mode = VK_POLYGON_MODE_LINE;
            break;

        case FillMode::Point:
            polygon_mode = VK_POLYGON_MODE_POINT;
            break;
    }

    switch (properties.winding_order) {
        case WindingOrder::CounterClockwise:
            front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            break;

        case WindingOrder::Clockwise:
            front_face = VK_FRONT_FACE_CLOCKWISE;
            break;
    }

    switch (properties.depth_compare_op) {
        case DepthCompareOp::Less:
            compare_op = VK_COMPARE_OP_LESS;
            break;

        case DepthCompareOp::LessOrEqual:
            compare_op = VK_COMPARE_OP_LESS_OR_EQUAL;
            break;

        case DepthCompareOp::Greater:
            compare_op = VK_COMPARE_OP_GREATER;
            break;

        case DepthCompareOp::GreaterOrEqual:
            compare_op = VK_COMPARE_OP_GREATER_OR_EQUAL;
            break;

        case DepthCompareOp::Equal:
            compare_op = VK_COMPARE_OP_EQUAL;
            break;

        case DepthCompareOp::Always:
            compare_op = VK_COMPARE_OP_ALWAYS;
            break;
    }

    //
    // Pipeline layout creation
    //
    VkPipelineDynamicStateCreateInfo dynamic_state_create_info {};
    dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

    dynamic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    dynamic_state_create_info.pDynamicStates = dynamic_states.data();


    VkPipelineVertexInputStateCreateInfo vertex_input_create_info {};
    vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    vertex_input_create_info.vertexBindingDescriptionCount = 1;
    vertex_input_create_info.pVertexBindingDescriptions = &vk_vtx_binding;

    vertex_input_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(vk_vtx_attributes.size());
    vertex_input_create_info.pVertexAttributeDescriptions = vk_vtx_attributes.data();

    // TODO: Other topologies?
    VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info {};
    input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_create_info.primitiveRestartEnable = VK_FALSE;

    // TODO: Multi-viewport? (for VR)
    VkPipelineViewportStateCreateInfo viewport_state_create_info{};
    viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    viewport_state_create_info.viewportCount = 1;
    viewport_state_create_info.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer_create_info{};
    rasterizer_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    // TODO: Depth bias?
    rasterizer_create_info.depthClampEnable = properties.clamp_depth;
    rasterizer_create_info.rasterizerDiscardEnable = properties.allow_discard;
    rasterizer_create_info.polygonMode = polygon_mode;
    rasterizer_create_info.lineWidth = 1.0f;
    rasterizer_create_info.cullMode = cull_flags;
    rasterizer_create_info.frontFace = front_face;
    rasterizer_create_info.depthBiasEnable = VK_FALSE;
    rasterizer_create_info.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer_create_info.depthBiasClamp = 0.0f; // Optional
    rasterizer_create_info.depthBiasSlopeFactor = 0.0f; // Optional

    // TODO: MSAA?
    VkPipelineMultisampleStateCreateInfo multisampling_create_info{};
    multisampling_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    multisampling_create_info.sampleShadingEnable = VK_FALSE;
    multisampling_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling_create_info.minSampleShading = 1.0f; // Optional
    multisampling_create_info.pSampleMask = nullptr; // Optional
    multisampling_create_info.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling_create_info.alphaToOneEnable = VK_FALSE; // Optional

    // TODO: Use color mask
    VkPipelineColorBlendAttachmentState color_blend_attachment_state{};

    color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment_state.blendEnable = VK_FALSE;
    color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    // TODO: Color blending?
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

    depth_stencil_state.depthTestEnable = properties.depth_test;
    depth_stencil_state.depthWriteEnable = properties.depth_write;
    depth_stencil_state.depthCompareOp = compare_op;
    depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_state.minDepthBounds = 0.0f; // Optional
    depth_stencil_state.maxDepthBounds = 1.0f; // Optional
    depth_stencil_state.stencilTestEnable = VK_FALSE;
    depth_stencil_state.front = {}; // Optional
    depth_stencil_state.back = {}; // Optional

    VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    // TODO: Tie the descriptor set type to the shader (for pipeline agnostic shaders?)

    // TODO: DESCRIPTOR SETS ASAP!!!
    std::vector<VkDescriptorSetLayout> vk_descriptor_set_layouts;

    pipeline_layout_create_info.setLayoutCount = static_cast<uint32_t>(vk_descriptor_set_layouts.size());
    pipeline_layout_create_info.pSetLayouts = vk_descriptor_set_layouts.data();
    pipeline_layout_create_info.pushConstantRangeCount = 0; // Optional
    pipeline_layout_create_info.pPushConstantRanges = nullptr; // Optional

    VkResult result = vkCreatePipelineLayout(p_provider->get_vk_device(), &pipeline_layout_create_info, nullptr, &vk_pipeline_layout);

    if (result != VK_SUCCESS) {
        LOG_GRAPHICS("Error: vkCreatePipelineLayout failed with error code (" << result << ")");
        throw std::runtime_error("vkCreatePipelineLayout failed! Please check the log above for more info!");
    }

    //
    // Pipeline creation
    //

    VkGraphicsPipelineCreateInfo pipeline_create_info{};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.stageCount = static_cast<uint32_t>(vk_stage_infos.size());
    pipeline_create_info.pStages = vk_stage_infos.data();
    pipeline_create_info.pVertexInputState = &vertex_input_create_info;
    pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
    pipeline_create_info.pViewportState = &viewport_state_create_info;
    pipeline_create_info.pRasterizationState = &rasterizer_create_info;
    pipeline_create_info.pMultisampleState = &multisampling_create_info;
    pipeline_create_info.pDepthStencilState = &depth_stencil_state; // Optional
    pipeline_create_info.pColorBlendState = &color_blend_state;
    pipeline_create_info.pDynamicState = &dynamic_state_create_info;
    pipeline_create_info.layout = vk_pipeline_layout;
    pipeline_create_info.renderPass = p_provider->get_render_pass_window(); // TODO: AGNOSTIC RENDER PASS ASAP!!!
    pipeline_create_info.subpass = 0; // TODO: Subpasses?
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipeline_create_info.basePipelineIndex = -1; // Optional

    result = vkCreateGraphicsPipelines(p_provider->get_vk_device(), nullptr, 1, &pipeline_create_info, nullptr, &vk_pipeline);

    if (result != VK_SUCCESS) {
        LOG_GRAPHICS("Error: vkCreateGraphicsPipelines failed with error code (" << result << ")");
        throw std::runtime_error("vkCreateGraphicsPipelines failed! Please check the log above for more info!");
    }
}

Graphics::Shader::Shader(VulkanProvider *p_provider, ShaderProperties properties, const std::shared_ptr<ShaderModule>& sm_vertex, const std::shared_ptr<ShaderModule>& sm_fragment) {
    if (sm_vertex == nullptr) {
        throw std::runtime_error("sm_vertex was nullptr!");
    }

    if (sm_fragment == nullptr) {
        throw std::runtime_error("sm_fragment was nullptr!");
    }

    compile(p_provider, properties, {sm_vertex, sm_fragment});
}

void Graphics::Shader::bind(VkCommandBuffer vk_cmd_buffer) {
    if (vk_cmd_buffer == nullptr) {
        throw std::runtime_error("vk_cmd_buffer was nullptr");
    }

    vkCmdBindPipeline(vk_cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline);
}
