#ifndef SAPPHIRE_VAL_RENDER_PASS_H
#define SAPPHIRE_VAL_RENDER_PASS_H

#include <vulkan/vulkan.h>

#include <rs_vulkan/val/val_releasable.h>

class ValRenderPass : public ValReleasable {
public:
    VkRenderPass vk_render_pass = nullptr;

    void release(ValInstance *p_val_instance) override;
};


#endif
