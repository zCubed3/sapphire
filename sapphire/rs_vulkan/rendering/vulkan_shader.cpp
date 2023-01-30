#include "vulkan_shader.h"

#include <engine/config/config_file.h>

#include <fstream>

#include <rs_vulkan/rendering/vulkan_render_server.h>
#include <rs_vulkan/val/val_instance.h>
#include <rs_vulkan/val/pipelines/val_shader_module.h>
#include <rs_vulkan/val/pipelines/val_pipeline_builder.h>

#include <rs_vulkan/shaders/error.spv.vert.gen.h>
#include <rs_vulkan/shaders/error.spv.frag.gen.h>

std::vector<char> read_file(const std::string& path) {
    std::vector<char> code;
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (file.is_open()) {
        code.resize(file.tellg());
        file.seekg(0);

        file.read(code.data(), code.size());
        file.close();
    }

    return code;
}

VulkanShader *VulkanShader::error_shader = nullptr;

VulkanShader::~VulkanShader() {
    const VulkanRenderServer* rs_instance = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());
    ValInstance* val_instance = rs_instance->val_instance;

    if (val_material_descriptor_set != nullptr) {
        val_material_descriptor_set->release(val_instance);
        delete val_material_descriptor_set;
    }

    if (val_object_descriptor_set != nullptr) {
        val_object_descriptor_set->release(val_instance);
        delete val_object_descriptor_set;
    }

    if (val_pipeline != nullptr) {
        val_pipeline->release(val_instance);
        delete val_pipeline;
    }
}

bool VulkanShader::make_from_semd(ConfigFile *p_semd_file) {
    if (p_semd_file == nullptr) {
        return false;
    }

    Shader::make_from_semd(p_semd_file);

    // TODO: Support arrays?
    std::vector<std::string> texture_params = p_semd_file->try_get_string_list("aTextureParameters", "Material");

    if (!texture_params.empty() && texture_params.size() % 2 == 0) {
        for (int t = 0; t < texture_params.size(); t += 2) {
            ShaderParameter parameter {};
            parameter.location = atoi(texture_params[t].c_str());
            parameter.stage_flags = VK_SHADER_STAGE_FRAGMENT_BIT;
            parameter.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

            parameters.push_back(parameter);
        }
    }

    std::string vert_path = p_semd_file->try_get_string("sVertBin", "VulkanShader");
    std::string frag_path = p_semd_file->try_get_string("sFragBin", "VulkanShader");

    if (vert_path.empty() || frag_path.empty()) {
        return false;
    }

    std::vector<char> vert_code = read_file(vert_path);
    std::vector<char> frag_code = read_file(frag_path);

    create_vert_frag(vert_code, frag_code);
    return true;
}

void VulkanShader::create_vert_frag(const std::vector<char> &vert_code, const std::vector<char> &frag_code) {
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

    // OpenGL culling mode
    switch (cull_mode) {
        case Shader::CULL_MODE_NONE:
            builder.cull_mode = ValPipelineBuilder::CULL_MODE_OFF;
            break;

        case Shader::CULL_MODE_BACK:
            builder.cull_mode = ValPipelineBuilder::CULL_MODE_BACK;
            break;

        case Shader::CULL_MODE_FRONT:
            builder.cull_mode = ValPipelineBuilder::CULL_MODE_FRONT;
            break;
    }

    switch (depth_op) {
        case Shader::DEPTH_OP_LESS:
            builder.depth_compare_op = ValPipelineBuilder::DEPTH_COMPARE_LESS;
            break;

        case Shader::DEPTH_OP_LESS_OR_EQUAL:
            builder.depth_compare_op = ValPipelineBuilder::DEPTH_COMPARE_LESS_OR_EQUAL;
            break;

        case Shader::DEPTH_OP_GREATER:
            builder.depth_compare_op = ValPipelineBuilder::DEPTH_COMPARE_GREATER;
            break;

        case Shader::DEPTH_OP_GREATER_OR_EQUAL:
            builder.depth_compare_op = ValPipelineBuilder::DEPTH_COMPARE_GREATER_OR_EQUAL;
            break;

        case Shader::DEPTH_OP_EQUAL:
            builder.depth_compare_op = ValPipelineBuilder::DEPTH_COMPARE_EQUAL;
            break;

        case Shader::DEPTH_OP_ALWAYS:
            builder.depth_compare_op = ValPipelineBuilder::DEPTH_COMPARE_ALWAYS;
            break;
    }

    builder.depth_write = write_depth;
    builder.winding_order = ValPipelineBuilder::WINDING_ORDER_COUNTER_CLOCKWISE;

    builder.push_module(vert_module);
    builder.push_module(frag_module);

    builder.val_render_pass = render_server->val_window_render_pass;

    // Our first set is the engine's "view" descriptor set
    builder.vk_descriptor_set_layouts.push_back(render_server->val_descriptor_info->vk_descriptor_set_layout);

    // The second set is unique to material instances
    // The third set is unique to object instances
    ValDescriptorSetBuilder set_builder {};

    // We need to push the parameters as they are ordered
    bool has_parameters = !parameters.empty();
    for (int i = 0; i < parameters.size(); i++) {
        for (ShaderParameter &parameter: parameters) {
            if (parameter.location == i) {
                set_builder.push_binding(parameter.type, 1, parameter.stage_flags);
            }
        }
    }

    // If we don't have any parameters we need to push a dummy
    if (!has_parameters) {
        set_builder.push_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
    }

    set_builder.push_set();

    set_builder.push_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

    std::vector<ValDescriptorSetInfo *> sets = set_builder.build(val_instance);

    val_material_descriptor_set = sets[0];
    val_object_descriptor_set = sets[1];

    // Since the object descriptor is unique per-object we can deallocate the set but keep the layout
    val_object_descriptor_set->release_set(val_instance);

    builder.vk_descriptor_set_layouts.push_back(val_material_descriptor_set->vk_descriptor_set_layout);
    builder.vk_descriptor_set_layouts.push_back(val_object_descriptor_set->vk_descriptor_set_layout);

    val_pipeline = builder.build(render_server->val_default_vertex_input, val_instance);

    vert_module->release(val_instance);
    delete vert_module;

    frag_module->release(val_instance);
    delete frag_module;
}

void VulkanShader::create_error_shader() {
    std::vector<char> vert_code;
    std::vector<char> frag_code;

    vert_code.resize(sizeof(ERROR_VERT_CONTENTS));
    frag_code.resize(sizeof(ERROR_FRAG_CONTENTS));

    memcpy(vert_code.data(), ERROR_VERT_CONTENTS, sizeof(ERROR_VERT_CONTENTS));
    memcpy(frag_code.data(), ERROR_FRAG_CONTENTS, sizeof(ERROR_FRAG_CONTENTS));

    VulkanShader* shader = new VulkanShader();
    shader->create_vert_frag(vert_code, frag_code);

    VulkanShader::error_shader = shader;
}

