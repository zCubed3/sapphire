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
#include <val/val_instance.h>
#include <val/pipelines/val_pipeline.h>

#include <vk_mem_alloc.h>

#ifdef DEBUG
#include <iostream>
#endif

//ValBuffer *VulkanMeshBuffer::transform_ubo = nullptr;

VulkanMeshBuffer::VulkanMeshBuffer(MeshAsset *p_mesh_asset) {
    const VulkanRenderServer* rs_instance = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());
    ValInstance* val_instance = rs_instance->val_instance;

    // TODO: Make the staging buffer more async?
    uint32_t vertex_count = 0;

    MeshAsset::Vertex *vertices = p_mesh_asset->get_vertex_data(&vertex_count);
    MeshAsset::index_t *indexes = p_mesh_asset->get_index_data(&index_count);

    uint32_t vbo_size = sizeof(MeshAsset::Vertex) * vertex_count;
    uint32_t ibo_size = sizeof(MeshAsset::index_t) * index_count;

    sub_ibo_offset = vbo_size;

    uint32_t mbo_size = vbo_size + ibo_size;
    ValStagingBuffer* mbo_staging = new ValStagingBuffer(mbo_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, val_instance);

    mbo_staging->write(vertices, val_instance, 0, vbo_size);
    mbo_staging->write(indexes, val_instance, vbo_size, ibo_size);

    VkCommandBuffer command_buffer = rs_instance->begin_upload();

    mbo_staging->copy_buffer(command_buffer);

    rs_instance->end_upload(command_buffer);

    val_mbo = mbo_staging->finalize(val_instance);

    mbo_staging->release(val_instance);
    delete mbo_staging;
}

VulkanMeshBuffer::~VulkanMeshBuffer() {
    const VulkanRenderServer* render_server = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());
    ValInstance* val_instance = render_server->val_instance;

    //val_object_descriptor_info->release(val_instance);
    //delete val_object_descriptor_info;

    if (val_mbo != nullptr) {
        val_mbo->release(val_instance);
        delete val_mbo;

#ifdef DEBUG
        std::cout << "Vulkan: 0x" << this << " released VulkanMeshBuffer::val_mbo" << std::endl;
#endif
    }
}

// TODO: Instancing
void VulkanMeshBuffer::draw(ObjectBuffer* p_object_buffer, std::shared_ptr<Material> p_material) {
    const VulkanRenderServer *render_server = reinterpret_cast<const VulkanRenderServer *>(RenderServer::get_singleton());
    ValInstance *val_instance = render_server->val_instance;

    // We assume a command buffer is currently recording
    VkCommandBuffer active_command_buffer = render_server->val_active_render_target->vk_command_buffer;

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(active_command_buffer, 0, 1, &val_mbo->vk_buffer, &offset);

    vkCmdBindIndexBuffer(active_command_buffer, val_mbo->vk_buffer, sub_ibo_offset, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(active_command_buffer, index_count, 1, 0, 0, 0);
}