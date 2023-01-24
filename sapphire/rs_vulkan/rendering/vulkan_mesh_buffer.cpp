#include "vulkan_mesh_buffer.h"

#include <vulkan/vulkan.h>

#include <rs_vulkan/assets/vulkan_shader_asset.h>
#include <rs_vulkan/rendering/vulkan_render_server.h>

VulkanMeshBuffer::VulkanMeshBuffer(MeshAsset *p_mesh_asset) {

}

// TODO: Instancing
void VulkanMeshBuffer::render(const Transform &transform, ShaderAsset *p_shader_asset) {
    MeshBuffer::render(transform, p_shader_asset);

    VulkanShaderAsset* vk_shader = reinterpret_cast<VulkanShaderAsset*>(p_shader_asset);
    const VulkanRenderServer* render_server = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)render_server->vk_extent.width;
    viewport.height = (float)render_server->vk_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = render_server->vk_extent;

    // We assume a command buffer is currently recording
    vkCmdBindPipeline(render_server->vk_active_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_shader->pipeline);

    vkCmdSetViewport(render_server->vk_active_command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(render_server->vk_active_command_buffer, 0, 1, &scissor);

    vkCmdDraw(render_server->vk_active_command_buffer, 3, 1, 0, 0);
}