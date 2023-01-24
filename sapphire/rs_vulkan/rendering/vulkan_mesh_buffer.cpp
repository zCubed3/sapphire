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

    size_t buffer_size = sizeof(Vertex) * p_mesh_asset->get_vertex_count();

    // TODO: Abstract this a bit
    VkBufferCreateInfo staging_buffer_info = {};
    staging_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    staging_buffer_info.size = buffer_size;
    staging_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VkBufferCreateInfo final_buffer_info = {};
    final_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    final_buffer_info.size = buffer_size;
    final_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    VmaAllocationCreateInfo staging_alloc_info = {};
    staging_alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
    staging_alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VmaAllocationCreateInfo final_alloc_info = {};
    final_alloc_info.usage = VMA_MEMORY_USAGE_AUTO;

    VmaAllocationInfo staging_allocation_info;
    VmaAllocation staging_allocation;
    VkBuffer staging_buffer;

    VmaAllocationInfo final_allocation_info;
    VmaAllocation final_allocation;

    vmaCreateBuffer(val_instance->vma_allocator, &staging_buffer_info, &staging_alloc_info, &staging_buffer, &staging_allocation, &staging_allocation_info);
    vmaCreateBuffer(val_instance->vma_allocator, &final_buffer_info, &final_alloc_info, &vk_buffer, &final_allocation, &final_allocation_info);

    void* data;
    vmaMapMemory(val_instance->vma_allocator, staging_allocation, &data);

    memcpy(data, vertices, buffer_size);

    vmaUnmapMemory(val_instance->vma_allocator, staging_allocation);

    VkCommandBuffer command_buffer = render_server->begin_upload();

    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = buffer_size;

    vkCmdCopyBuffer(command_buffer, staging_buffer, vk_buffer, 1, &copy_region);

    render_server->end_upload(command_buffer);

    delete[] vertices;
}

// TODO: Instancing
void VulkanMeshBuffer::render(const Transform &transform, ShaderAsset *p_shader_asset) {
    MeshBuffer::render(transform, p_shader_asset);

    VulkanShaderAsset* vk_shader = reinterpret_cast<VulkanShaderAsset*>(p_shader_asset);

    const VulkanRenderServer* render_server = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());
    ValWindow* window = render_server->val_instance->val_main_window;

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
    vkCmdBindPipeline(window->vk_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_shader->pipeline);

    vkCmdSetViewport(window->vk_command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(window->vk_command_buffer, 0, 1, &scissor);

    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(window->vk_command_buffer, 0, 1, &vk_buffer, &offset);

    vkCmdDraw(window->vk_command_buffer, tri_count, 1, 0, 0);
}