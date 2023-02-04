#include "vulkan_texture.h"

#include <rs_vulkan/rendering/vulkan_render_server.h>
#include <val/data/val_buffer.h>
#include <val/images/val_image.h>

#if defined(IMGUI_SUPPORT)
#include <backends/imgui_impl_vulkan.h>
#endif

#ifdef DEBUG
#include <iostream>
#endif

VulkanTexture::~VulkanTexture() {
    const VulkanRenderServer *rs_instance = reinterpret_cast<const VulkanRenderServer *>(RenderServer::get_singleton());

    if (val_image != nullptr && owns_image) {
        val_image->release(rs_instance->val_instance);
        delete val_image;

#ifdef DEBUG
        std::cout << "Vulkan: 0x" << this << " released VulkanTexture::val_image" << std::endl;
#endif
    }

#if defined(IMGUI_SUPPORT)
    if (val_imgui_descriptor_set != nullptr) {
        val_imgui_descriptor_set->release(rs_instance->val_instance);
        delete val_imgui_descriptor_set;

#ifdef DEBUG
        std::cout << "Vulkan: 0x" << this << " released VulkanTexture::val_imgui_descriptor_set" << std::endl;
#endif
    }
#endif
}

VulkanTexture::VulkanTexture(ValImage *val_image, bool owns_image) {
    this->val_image = val_image;
    this->owns_image = owns_image;
}

#if defined(IMGUI_SUPPORT)
void *VulkanTexture::get_imgui_handle() {
    const VulkanRenderServer *rs_instance = reinterpret_cast<const VulkanRenderServer *>(RenderServer::get_singleton());

    if (val_imgui_descriptor_set == nullptr) {
        val_imgui_descriptor_set = rs_instance->val_imgui_descriptor_info->allocate_set(rs_instance->val_instance);
    }

    ValDescriptorSetWriteInfo write_info {};
    write_info.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write_info.val_image = val_image;

    val_imgui_descriptor_set->write_binding(&write_info);
    val_imgui_descriptor_set->update_set(rs_instance->val_instance);

    return (void*)val_imgui_descriptor_set->vk_descriptor_set;
}
#endif

void VulkanTexture::load_bytes(unsigned char *bytes, int width, int height, int channels) {
    // We need to create 2 parts a staging buffer and an image
    // TODO: Move this into Val and not here?
    const VulkanRenderServer *rs_instance = reinterpret_cast<const VulkanRenderServer *>(RenderServer::get_singleton());

    ValImageCreateInfo create_info{};
    create_info.generate_mips = true; // TODO: Optional?
    create_info.extent.width = width;
    create_info.extent.height = height;
    create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    create_info.usage_flags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    val_image = ValImage::create(&create_info, rs_instance->val_instance);

    // We then need to transfer the image contents into a buffer
    VkCommandBuffer vk_command_buffer = rs_instance->begin_upload(false);

    val_image->write_bytes(bytes, vk_command_buffer, rs_instance->val_instance);

    rs_instance->end_upload(vk_command_buffer, false);

    val_image->post_write(rs_instance->val_instance);
}