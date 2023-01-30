#ifndef SAPPHIRE_VAL_TEXTURE_H
#define SAPPHIRE_VAL_TEXTURE_H

#include <rs_vulkan/val/val_releasable.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

// Wrapper around images meant for texturing
struct ValTextureCreateInfo {
    enum Dimensions {
        DIMENSIONS_1D,
        DIMENSIONS_2D,
        DIMENSIONS_3D,
        DIMENSIONS_CUBE
    };

    VkExtent3D extent {1, 1, 1};
    uint32_t array_size = 1;
    uint32_t mip_levels = 1;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
    VkImageUsageFlags usage_flags = 0;
    Dimensions dimensions = DIMENSIONS_2D;
    VkImageAspectFlags aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;
};

class ValTexture : public ValReleasable {
public:
    VkImage *vk_image = nullptr;
    VmaAllocation *vma_allocation = nullptr;

    static ValTexture *create(ValTextureCreateInfo *p_create_info, ValInstance *p_val_instance);
};


#endif
