#ifndef SAPPHIRE_VULKAN_RENDER_TARGET_BACKING_H
#define SAPPHIRE_VULKAN_RENDER_TARGET_BACKING_H

// -- DISCLAIMER --
// In vulkan a render target is composed of 3 major parts
// 1) VkImage(s) (the actual texture memory written to)
// 2) VkImageView(s) (the info on how to read / write to the texture)
// 3) VkFramebuffer(s) (the GPU data glue)
// ----------------

#include <vector>
#include <vulkan/vulkan.h>

class VulkanRenderTargetBacking {
public:
    std::vector<VkImage> images;
    std::vector<VkImageView> image_views;
    std::vector<VkFramebuffer> framebuffers;

    VkExtent2D extent;
};

#endif
