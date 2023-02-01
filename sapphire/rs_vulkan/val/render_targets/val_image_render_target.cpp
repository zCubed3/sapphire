#include "val_image_render_target.h"

#include <rs_vulkan/val/val_instance.h>
#include <rs_vulkan/val/pipelines/val_render_pass.h>

VkFramebuffer ValImageRenderTarget::get_framebuffer(ValInstance *p_val_instance) {
    return vk_framebuffer;
}

// TODO: Not use a constructor (for error catching)
ValImageRenderTarget::ValImageRenderTarget(ValRenderTargetCreateInfo *p_create_info, ValInstance *p_val_instance) {
    creation_extent = p_create_info->extent;

    ValImageCreateInfo color_create_info {};
    color_create_info.format = p_create_info->format;
    color_create_info.extent.width = p_create_info->extent.width;
    color_create_info.extent.height = p_create_info->extent.height;
    color_create_info.usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    color_create_info.aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;

    ValImageCreateInfo depth_create_info {};
    depth_create_info.extent.width = p_create_info->extent.width;
    depth_create_info.extent.height = p_create_info->extent.height;
    depth_create_info.format = p_val_instance->present_info->vk_depth_format;
    depth_create_info.usage_flags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depth_create_info.aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;

    val_color_image = ValImage::create(&color_create_info, p_val_instance);
    val_depth_image = ValImage::create(&depth_create_info, p_val_instance);

    // We need to create just a single framebuffer
    std::vector<VkImageView> attachments = {
            val_color_image->vk_image_view,
            val_depth_image->vk_image_view
    };

    VkFramebufferCreateInfo framebuffer_create_info{};
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.renderPass = p_create_info->val_render_pass->vk_render_pass;
    framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebuffer_create_info.pAttachments = attachments.data();
    framebuffer_create_info.width = p_create_info->extent.width;
    framebuffer_create_info.height = p_create_info->extent.height;
    framebuffer_create_info.layers = 1;

    if (vkCreateFramebuffer(p_val_instance->vk_device, &framebuffer_create_info, nullptr, &vk_framebuffer) != VK_SUCCESS) {
        // TODO: Error, failed to create framebuffer!
    }
}