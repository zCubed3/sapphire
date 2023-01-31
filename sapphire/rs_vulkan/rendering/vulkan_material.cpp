#include "vulkan_material.h"

#include <rs_vulkan/rendering/vulkan_render_server.h>
#include <rs_vulkan/rendering/vulkan_shader.h>
#include <rs_vulkan/rendering/vulkan_texture.h>

#include <rs_vulkan/val/pipelines/val_pipeline.h>

void VulkanMaterial::bind() {
    const VulkanRenderServer *rs_instance = reinterpret_cast<const VulkanRenderServer *>(RenderServer::get_singleton());

    VkCommandBuffer active_command_buffer = rs_instance->val_active_render_target->vk_command_buffer;
    VulkanShader *vk_shader = reinterpret_cast<VulkanShader *>(shader);

    if (val_material_descriptor_info == nullptr && vk_shader->val_material_descriptor_set != nullptr) {
        val_material_descriptor_info = vk_shader->val_material_descriptor_set->allocate_set(rs_instance->val_instance);
    }

    if (val_material_descriptor_info != nullptr) {
        if (!shader->parameters.empty()) {
            /*VulkanTexture *test_tex = reinterpret_cast<VulkanTexture *>(vk_shader->texture_asset->texture);

            ValDescriptorSetWriteInfo material_write_info{};
            material_write_info.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            material_write_info.val_image = test_tex->val_image;

            val_material_descriptor_info->write_binding(&material_write_info);
            val_material_descriptor_info->update_set(val_instance);*/
        }
    }

    if (vk_shader->val_material_descriptor_set != nullptr) {
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