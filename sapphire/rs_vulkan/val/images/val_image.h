#ifndef SAPPHIRE_VAL_IMAGE_H
#define SAPPHIRE_VAL_IMAGE_H

#include <rs_vulkan/val/val_releasable.h>
#include <vulkan/vulkan.h>

struct ValImageCreateInfo {
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
};

// Wrapper around a texture
class ValImage : public ValReleasable {
protected:
    ValImage() = default;

public:
    VkImage vk_image = nullptr;
    VkImageView vk_image_view = nullptr;
    VkDeviceMemory vk_image_memory = nullptr;

    static ValImage *create(ValImageCreateInfo *p_create_info, ValInstance *p_val_instance);

    void release(ValInstance *p_val_instance) override;
};


#endif
