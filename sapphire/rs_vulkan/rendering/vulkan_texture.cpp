#include "vulkan_texture.h"

#include <rs_vulkan/rendering/vulkan_render_server.h>
#include <rs_vulkan/val/data/val_buffer.h>
#include <rs_vulkan/val/images/val_image.h>

VulkanTexture::~VulkanTexture() {
    const VulkanRenderServer *rs_instance = reinterpret_cast<const VulkanRenderServer *>(RenderServer::get_singleton());

    if (val_image != nullptr) {
        val_image->release(rs_instance->val_instance);
        delete val_image;
    }
}

void VulkanTexture::load_bytes(unsigned char *bytes, int width, int height, int channels) {
    // We need to create 2 parts a staging buffer and an image
    // TODO: Move this into Val and not here?
    const VulkanRenderServer *rs_instance = reinterpret_cast<const VulkanRenderServer *>(RenderServer::get_singleton());

    ValImageCreateInfo create_info{};
    create_info.extent.width = width;
    create_info.extent.height = height;
    create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    create_info.usage_flags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    val_image = ValImage::create(&create_info, rs_instance->val_instance);

    // We then need to transfer the image contents into a buffer
    VkCommandBuffer vk_command_buffer = rs_instance->begin_upload(false);

    val_image->write_bytes(bytes, vk_command_buffer, rs_instance->val_instance);

    rs_instance->end_upload(vk_command_buffer, false);

    val_image->post_write(rs_instance->val_instance);
}
