#include "vulkan_render_target_data.h"

#include <engine/rendering/render_target.h>

#include <rs_vulkan/rendering/vulkan_render_server.h>
#include <rs_vulkan/rendering/vulkan_texture.h>
#include <rs_vulkan/val/render_targets/val_image_render_target.h>

VulkanRenderTargetData::VulkanRenderTargetData(ValRenderTarget *p_val_render_target) {
    val_render_target = p_val_render_target;
}

VulkanRenderTargetData::~VulkanRenderTargetData() {
    RenderTargetData::~RenderTargetData();

    if (val_render_target != nullptr) {
        const VulkanRenderServer* render_server = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());
        ValInstance* val_instance = render_server->val_instance;

        val_render_target->release(val_instance);
        delete val_render_target;
    }
}

void VulkanRenderTargetData::resize(int width, int height, RenderTarget* p_target) {
    if (p_target->get_type() == RenderTarget::TARGET_TYPE_TEXTURE) {
        const VulkanRenderServer* rs_instance = reinterpret_cast<const VulkanRenderServer*>(RenderServer::get_singleton());
        ValImageRenderTarget *image_target = reinterpret_cast<ValImageRenderTarget*>(val_render_target);

        val_render_target->resize(width, height, rs_instance->val_instance);

        delete color_texture;
        delete depth_texture;

        color_texture = new VulkanTexture(image_target->val_color_image, false);
        depth_texture = new VulkanTexture(image_target->val_depth_image, false);
    }
}

Texture *VulkanRenderTargetData::get_color_texture() {
    return color_texture;
}

Texture *VulkanRenderTargetData::get_depth_texture() {
    return depth_texture;
}