#include "vulkan_material.h"

#include <engine/assets/texture_asset.h>

#include <rs_vulkan/rendering/vulkan_render_server.h>
#include <rs_vulkan/rendering/vulkan_shader.h>
#include <rs_vulkan/rendering/vulkan_texture.h>

#include <rs_vulkan/val/pipelines/val_pipeline.h>
#include <rs_vulkan/val/images/val_image.h>

void VulkanMaterial::bind() {
    const VulkanRenderServer *rs_instance = reinterpret_cast<const VulkanRenderServer *>(RenderServer::get_singleton());

    VkCommandBuffer active_command_buffer = rs_instance->val_active_render_target->vk_command_buffer;
    VulkanShader *vk_shader = reinterpret_cast<VulkanShader *>(shader);

    if (vk_shader != nullptr) {
        if (val_material_descriptor_info == nullptr && vk_shader->val_material_descriptor_set != nullptr) {
            val_material_descriptor_info = vk_shader->val_material_descriptor_set->allocate_set(rs_instance->val_instance);
        }
    }

    if (val_material_descriptor_info != nullptr) {
        if (!shader->parameters.empty()) {
            // Whenever we apply a parameter we check if it has been overriden
            for (Shader::ShaderParameter& param: shader->parameters) {
                void* data = nullptr;
                for (Shader::ShaderParameter& override_param: parameter_overrides) {
                    if (override_param.name == param.name) {
                        data = override_param.data;
                        break;
                    }
                }

                if (data == nullptr) {
                    data = param.data;
                }

                if (param.type == Shader::SHADER_PARAMETER_TEXTURE) {
                    // TODO: Not reference assets and instead reference the underlying texture
                    TextureAsset* texture = reinterpret_cast<TextureAsset*>(data);
                    VulkanTexture* vulkan_texture = reinterpret_cast<VulkanTexture*>(texture->texture);

                    ValDescriptorSetWriteInfo write_info {};
                    write_info.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    write_info.binding_index = param.location;
                    write_info.val_image = vulkan_texture->val_image;

                    val_material_descriptor_info->write_binding(&write_info);
                }
            }

            val_material_descriptor_info->update_set(rs_instance->val_instance);

            vkCmdBindDescriptorSets(
                    active_command_buffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    vk_shader->val_pipeline->vk_pipeline_layout,
                    1,
                    1,
                    &val_material_descriptor_info->vk_descriptor_set,
                    0,
                    nullptr);
        }
    }
}