#include "val_image_render_target.h"

VkFramebuffer ValImageRenderTarget::get_framebuffer(ValInstance *p_val_instance) {
    return vk_framebuffer;
}

// TODO: Not use a constructor (for error catching)
ValImageRenderTarget::ValImageRenderTarget(ValRenderTargetCreateInfo *p_create_info, ValInstance *p_val_instance) {

}