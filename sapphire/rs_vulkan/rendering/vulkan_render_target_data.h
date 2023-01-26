#ifndef SAPPHIRE_VULKAN_RENDER_TARGET_DATA_H
#define SAPPHIRE_VULKAN_RENDER_TARGET_DATA_H

#include <vector>
#include <vulkan/vulkan.h>

#include <engine/rendering/render_target_data.h>

#include <rs_vulkan/val/render_targets/val_render_target.h>

class VulkanRenderTargetData : public RenderTargetData {
public:
    ValRenderTarget *val_render_target = nullptr;

    VulkanRenderTargetData(ValRenderTarget *p_val_render_target);
    ~VulkanRenderTargetData() override;
};

#endif
