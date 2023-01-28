#include "val_render_pass.h"

#include <rs_vulkan/val/val_instance.h>

void ValRenderPass::release(ValInstance *p_val_instance) {
    ValReleasable::release(p_val_instance);

    if (vk_render_pass != nullptr) {
        vkDestroyRenderPass(p_val_instance->vk_device, vk_render_pass, nullptr);
        vk_render_pass = nullptr;
    }
}
