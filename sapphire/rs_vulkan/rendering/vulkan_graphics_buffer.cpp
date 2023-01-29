#include "vulkan_graphics_buffer.h"

#include <rs_vulkan/rendering/vulkan_render_server.h>

VulkanGraphicsBuffer::VulkanGraphicsBuffer(size_t size) {
    const VulkanRenderServer *vulkan_rs = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());

    val_buffer = new ValBuffer(
            size,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            vulkan_rs->val_instance);
}

void VulkanGraphicsBuffer::write(void *data, size_t size) {
    const VulkanRenderServer *vulkan_rs = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());

    val_buffer->write(data, vulkan_rs->val_instance);
}