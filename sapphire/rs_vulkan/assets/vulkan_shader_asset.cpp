#include "vulkan_shader_asset.h"

#include <rs_vulkan/rendering/vulkan_render_server.h>
#include <rs_vulkan/rendering/vulkan_mesh_buffer.h>

#include <rs_vulkan/val/val_instance.h>
#include <rs_vulkan/val/pipelines/val_shader_module.h>
#include <rs_vulkan/val/pipelines/val_pipeline_builder.h>

void VulkanShaderAsset::create_vert_frag(const std::vector<char> &vert_code, const std::vector<char> &frag_code) {
    const VulkanRenderServer *render_server = reinterpret_cast<const VulkanRenderServer *>(RenderServer::get_singleton());
    ValInstance *val_instance = render_server->val_instance;

    ValPipelineBuilder builder{};

    ValShaderModuleCreateInfo vert_module_info{};
    vert_module_info.stage = ValShaderModuleCreateInfo::STAGE_VERTEX;
    vert_module_info.code = vert_code;

    ValShaderModuleCreateInfo frag_module_info{};
    frag_module_info.stage = ValShaderModuleCreateInfo::STAGE_FRAGMENT;
    frag_module_info.code = frag_code;

    ValShaderModule *vert_module = ValShaderModule::create_shader_module(&vert_module_info, val_instance);
    ValShaderModule *frag_module = ValShaderModule::create_shader_module(&frag_module_info, val_instance);

    builder.push_module(vert_module);
    builder.push_module(frag_module);

    builder.val_render_pass = render_server->val_window_render_pass;
    builder.val_descriptor_set = render_server->val_descriptor_set;

    val_pipeline = builder.build(render_server->val_default_vertex_input, val_instance);

    vert_module->release(val_instance);
    delete vert_module;

    frag_module->release(val_instance);
    delete frag_module;
}

VulkanShaderAsset::~VulkanShaderAsset() {
    const VulkanRenderServer* render_server = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());
    ValInstance* val_instance = render_server->val_instance;

    if (val_pipeline != nullptr) {
        val_pipeline->release(val_instance);
        delete val_pipeline;
    }
}