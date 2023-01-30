#ifndef SAPPHIRE_VAL_IMAGE_H
#define SAPPHIRE_VAL_IMAGE_H

#include <rs_vulkan/val/val_releasable.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

struct ValSamplerCreateInfo {
    VkFilter mag_filter = VK_FILTER_LINEAR;
    VkFilter min_filter = VK_FILTER_LINEAR;

    VkSamplerAddressMode mode_u = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode mode_v = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode mode_w = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    VkSamplerMipmapMode mip_mode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

};

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

    Dimensions dimensions = DIMENSIONS_2D;

    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
    VkImageUsageFlags usage_flags = 0;
    VkImageAspectFlags aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT;

    bool create_sampler = true;
    ValSamplerCreateInfo sampler_info {};
};

class ValBuffer;

// Wrapper around a texture
class ValImage : public ValReleasable {
protected:
    ValImage() = default;
    ValBuffer* val_buffer = nullptr;

public:
    VkExtent3D vk_extent {};
    VkImage vk_image = nullptr;
    VkImageView vk_image_view = nullptr;
    VkSampler vk_sampler = nullptr;
    VmaAllocation vma_allocation = nullptr;
    VkImageAspectFlags vk_aspect_flags = 0;

    static ValImage *create(ValImageCreateInfo *p_create_info, ValInstance *p_val_instance);

    void write_bytes(unsigned char *bytes, VkCommandBuffer vk_command_buffer, ValInstance *p_val_instance);
    void transfer_layout(VkImageLayout old_layout, VkImageLayout new_layout, VkCommandBuffer vk_command_buffer);
    void post_write(ValInstance *p_val_instance);

    void release(ValInstance *p_val_instance) override;
};


#endif
