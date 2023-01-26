#include "vulkan_render_target_data.h"

#include <rs_vulkan/rendering/vulkan_render_server.h>

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
