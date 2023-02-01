#ifndef SAPPHIRE_VAL_IMAGE_RENDER_TARGET_H
#define SAPPHIRE_VAL_IMAGE_RENDER_TARGET_H

#include <rs_vulkan/val/render_targets/val_render_target.h>
#include <rs_vulkan/val/images/val_image.h>

class ValImageRenderTarget : public ValRenderTarget {
protected:
    VkFramebuffer get_framebuffer(ValInstance *p_val_instance) override;

public:
    VkFramebuffer vk_framebuffer = nullptr;
    ValImage* val_color_image = nullptr;
    ValImage* val_depth_image = nullptr;

    ValImageRenderTarget(ValRenderTargetCreateInfo *p_create_info, ValInstance *p_val_instance);
};


#endif
