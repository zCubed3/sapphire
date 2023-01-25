#include "vulkan_mesh_buffer.h"

#include <vulkan/vulkan.h>

#include <engine/assets/mesh_asset.h>
#include <rs_vulkan/assets/vulkan_shader_asset.h>
#include <rs_vulkan/rendering/vulkan_render_server.h>
#include <rs_vulkan/val/val_instance.h>

#include <vk_mem_alloc.h>

VulkanMeshBuffer::VulkanMeshBuffer(MeshAsset *p_mesh_asset) {
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

    const VulkanRenderServer* render_server = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());
    ValInstance* val_instance = render_server->val_instance;

    size_t vbo_size = sizeof(Vertex) * p_mesh_asset->get_vertex_count();
    size_t ibo_size = sizeof(uint32_t) * p_mesh_asset->get_triangle_count();

    ValStagingBuffer* vbo_staging = new ValStagingBuffer(vbo_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, val_instance);
    ValStagingBuffer* ibo_staging = new ValStagingBuffer(ibo_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, val_instance);

    vbo_staging->write(vertices, val_instance);
    ibo_staging->write(triangles, val_instance);

    VkCommandBuffer command_buffer = render_server->begin_upload();

    vbo_staging->copy_buffer(command_buffer);
    ibo_staging->copy_buffer(command_buffer);

    render_server->end_upload(command_buffer);

    val_vbo = vbo_staging->finalize(val_instance);
    val_ibo = ibo_staging->finalize(val_instance);

    vbo_staging->release(val_instance);
    delete vbo_staging;

    ibo_staging->release(val_instance);
    delete ibo_staging;
    
    delete[] vertices;
}

// TODO: Instancing
void VulkanMeshBuffer::render(const Transform &transform, ShaderAsset *p_shader_asset) {
    MeshBuffer::render(transform, p_shader_asset);

    VulkanShaderAsset* vk_shader = reinterpret_cast<VulkanShaderAsset*>(p_shader_asset);

    const VulkanRenderServer* render_server = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());
    ValInstance* val_instance = render_server->val_instance;
    ValWindow* window = val_instance->val_main_window;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)window->vk_extent.width;
    viewport.height = (float)window->vk_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = window->vk_extent;

    // TODO: Not assume we're always rendering to the main window
    // We assume a command buffer is currently recording
    vkCmdBindPipeline(window->vk_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_shader->vk_pipeline);

    vkCmdSetViewport(window->vk_command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(window->vk_command_buffer, 0, 1, &scissor);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(window->vk_command_buffer, 0, 1, &val_vbo->vk_buffer, &offset);

    vkCmdBindIndexBuffer(window->vk_command_buffer, val_ibo->vk_buffer, offset, VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(
            window->vk_command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            vk_shader->vk_pipeline_layout,
            0,
            1,
            &render_server->vk_descriptor_set,
            0,
            nullptr);

    vkCmdDrawIndexed(window->vk_command_buffer, tri_count, 1, 0, 0, 0);
}

VulkanMeshBuffer::~VulkanMeshBuffer() {
    const VulkanRenderServer* render_server = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());
    ValInstance* val_instance = render_server->val_instance;

    val_vbo->release(val_instance);
    delete val_vbo;

    val_ibo->release(val_instance);
    delete val_ibo;
}