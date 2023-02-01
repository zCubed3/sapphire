#include "vulkan_mesh_buffer.h"

#include <vulkan/vulkan.h>

#include <engine/assets/material_asset.h>
#include <engine/assets/mesh_asset.h>
#include <engine/assets/texture_asset.h>
#include <engine/rendering/buffers/object_buffer.h>
#include <engine/rendering/render_target.h>
#include <engine/rendering/texture.h>

#include <rs_vulkan/rendering/vulkan_render_server.h>
#include <rs_vulkan/rendering/vulkan_graphics_buffer.h>
#include <rs_vulkan/rendering/vulkan_shader.h>
#include <rs_vulkan/rendering/vulkan_material.h>
#include <rs_vulkan/rendering/vulkan_texture.h>
#include <rs_vulkan/val/val_instance.h>
#include <rs_vulkan/val/pipelines/val_pipeline.h>

#include <vk_mem_alloc.h>

//ValBuffer *VulkanMeshBuffer::transform_ubo = nullptr;

VulkanMeshBuffer::VulkanMeshBuffer(MeshAsset *p_mesh_asset) {
    const VulkanRenderServer* rs_instance = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());
    ValInstance* val_instance = rs_instance->val_instance;

    // TODO: Make the staging buffer more async?
    Vertex *vertices = new Vertex[p_mesh_asset->get_vertex_count()];

    uint32_t *triangles = p_mesh_asset->get_triangle_data(nullptr);
    glm::vec3 *positions = p_mesh_asset->get_position_data(nullptr);
    glm::vec3 *normals = p_mesh_asset->get_normal_data(nullptr);
    glm::vec2 *tex_coords = p_mesh_asset->get_uv0_data(nullptr);

    tri_count = p_mesh_asset->get_triangle_count();

    for (uint32_t v = 0; v < p_mesh_asset->get_vertex_count(); v++) {
        if (positions != nullptr) {
            vertices[v].position = positions[v];
        }

        if (normals != nullptr) {
            vertices[v].normal = normals[v];
        }

        if (tex_coords != nullptr) {
            vertices[v].uv0 = tex_coords[v];
        }
    }

    size_t vbo_size = sizeof(Vertex) * p_mesh_asset->get_vertex_count();
    size_t ibo_size = sizeof(uint32_t) * p_mesh_asset->get_triangle_count();

    sub_ibo_offset = vbo_size;

    size_t mbo_size = vbo_size + ibo_size;

    ValStagingBuffer* mbo_staging = new ValStagingBuffer(mbo_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, val_instance);

    mbo_staging->write(vertices, val_instance, 0, vbo_size);
    mbo_staging->write(triangles, val_instance, vbo_size, ibo_size);

    VkCommandBuffer command_buffer = rs_instance->begin_upload();

    mbo_staging->copy_buffer(command_buffer);

    rs_instance->end_upload(command_buffer);

    val_mbo = mbo_staging->finalize(val_instance);

    mbo_staging->release(val_instance);
    delete mbo_staging;
    
    delete[] vertices;
}

// TODO: Instancing
void VulkanMeshBuffer::render(ObjectBuffer* p_object_buffer, Material *p_material) {
    // We expect the material to have been bound and updated
    // Therefore we just simply grab the shader
    VulkanShader *vk_shader = nullptr;
    if (p_material != nullptr) {
        // TODO: Actually do what stupid past me said
        p_material->bind();
        vk_shader = reinterpret_cast<VulkanShader *>(p_material->shader);
    }

    if (vk_shader == nullptr) {
        vk_shader = VulkanShader::error_shader;
    }

    const VulkanRenderServer *render_server = reinterpret_cast<const VulkanRenderServer *>(RenderServer::get_singleton());
    ValInstance *val_instance = render_server->val_instance;

    RenderTarget *current_target = render_server->get_current_target();

    // If we don't have an instance of the per-object data we must allocate one
    if (val_object_descriptor_info == nullptr) {
        val_object_descriptor_info = vk_shader->val_object_descriptor_set->allocate_set(val_instance);
    }
    // Our object data is already updated by this moment
    // We just have to update the binding
    VulkanGraphicsBuffer *object_ubo = reinterpret_cast<VulkanGraphicsBuffer *>(p_object_buffer->buffer);

    ValDescriptorSetWriteInfo object_write_info{};
    object_write_info.val_buffer = object_ubo->val_buffer;

    val_object_descriptor_info->write_binding(&object_write_info);
    val_object_descriptor_info->update_set(val_instance);

    Rect rect = current_target->get_rect();

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) rect.width;
    viewport.height = (float) rect.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {static_cast<uint32_t>(rect.width), static_cast<uint32_t>(rect.height)};

    // We assume a command buffer is currently recording
    VkCommandBuffer active_command_buffer = render_server->val_active_render_target->vk_command_buffer;

    vkCmdBindPipeline(active_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_shader->val_pipeline->vk_pipeline);

    vkCmdSetViewport(active_command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(active_command_buffer, 0, 1, &scissor);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(active_command_buffer, 0, 1, &val_mbo->vk_buffer, &offset);

    vkCmdBindIndexBuffer(active_command_buffer, val_mbo->vk_buffer, sub_ibo_offset, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(
            active_command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            vk_shader->val_pipeline->vk_pipeline_layout,
            2,
            1,
            &val_object_descriptor_info->vk_descriptor_set,
            0,
            nullptr);

    vkCmdDrawIndexed(active_command_buffer, tri_count, 1, 0, 0, 0);
}

VulkanMeshBuffer::~VulkanMeshBuffer() {
    const VulkanRenderServer* render_server = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());
    ValInstance* val_instance = render_server->val_instance;

    val_object_descriptor_info->release(val_instance);
    delete val_object_descriptor_info;

    val_mbo->release(val_instance);
    delete val_mbo;
}