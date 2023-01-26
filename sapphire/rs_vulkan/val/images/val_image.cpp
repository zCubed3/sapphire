#include "val_image.h"

#include <rs_vulkan/val/val_instance.h>

ValImage *ValImage::create(ValImageCreateInfo *p_create_info, ValInstance *p_val_instance) {
    if (p_create_info == nullptr) {
        return nullptr;
    }

    VkImageType image_type;
    VkImageViewType view_type;

    switch (p_create_info->dimensions) {
        case ValImageCreateInfo::DIMENSIONS_1D:
            image_type = VK_IMAGE_TYPE_1D;
            view_type = VK_IMAGE_VIEW_TYPE_1D;
            break;

        case ValImageCreateInfo::DIMENSIONS_2D:
            image_type = VK_IMAGE_TYPE_2D;
            view_type = VK_IMAGE_VIEW_TYPE_2D;
            break;

        case ValImageCreateInfo::DIMENSIONS_3D:
            image_type = VK_IMAGE_TYPE_3D;
            view_type = VK_IMAGE_VIEW_TYPE_3D;
            break;

        case ValImageCreateInfo::DIMENSIONS_CUBE:
            image_type = VK_IMAGE_TYPE_2D;
            view_type = VK_IMAGE_VIEW_TYPE_CUBE;
            break;
    }

    if (p_create_info->dimensions == ValImageCreateInfo::DIMENSIONS_CUBE) {
        p_create_info->array_size = 6;
    }

    VkImageCreateInfo image_info {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = image_type;
    image_info.extent.width = p_create_info->extent.width;
    image_info.extent.height = p_create_info->extent.height;
    image_info.extent.depth = p_create_info->extent.depth;
    image_info.mipLevels = p_create_info->mip_levels;
    image_info.arrayLayers = p_create_info->array_size;
    image_info.samples = p_create_info->samples;
    image_info.format = p_create_info->format;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL; // TODO: Do we need other image tiling types?
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // TODO: Do we need other layouts?
    image_info.usage = p_create_info->usage_flags;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Do we need other sharing modes?

    VkImage vk_image = nullptr;
    if (vkCreateImage(p_val_instance->vk_device, &image_info, nullptr, &vk_image) != VK_SUCCESS) {
        return nullptr;
    }

    VkMemoryRequirements mem_requirements {};
    vkGetImageMemoryRequirements(p_val_instance->vk_device, vk_image, &mem_requirements);

    VmaAllocationCreateInfo depth_info {};
    uint32_t memory_type;
    vmaFindMemoryTypeIndex(p_val_instance->vma_allocator, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &depth_info, &memory_type);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = memory_type;

    VkDeviceMemory vk_image_memory = nullptr;
    if (vkAllocateMemory(p_val_instance->vk_device, &alloc_info, nullptr, &vk_image_memory) != VK_SUCCESS) {
        vkDestroyImage(p_val_instance->vk_device, vk_image, nullptr);

        return nullptr;
    }

    vkBindImageMemory(p_val_instance->vk_device, vk_image, vk_image_memory, 0);

    VkImageViewCreateInfo image_view_info{};
    image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    image_view_info.image = vk_image;
    image_view_info.viewType = view_type;
    image_view_info.format = p_create_info->format;

    image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    image_view_info.subresourceRange.baseMipLevel = 0;
    image_view_info.subresourceRange.levelCount = p_create_info->mip_levels;
    image_view_info.subresourceRange.baseArrayLayer = 0;
    image_view_info.subresourceRange.layerCount = 1;

    VkImageView vk_image_view = nullptr;
    if (vkCreateImageView(p_val_instance->vk_device, &image_view_info, nullptr, &vk_image_view) != VK_SUCCESS) {
        vkDestroyImage(p_val_instance->vk_device, vk_image, nullptr);
        vkFreeMemory(p_val_instance->vk_device, vk_image_memory, nullptr);

        return nullptr;
    }

    ValImage* image = new ValImage();
    image->vk_image = vk_image;
    image->vk_image_view = vk_image_view;
    image->vk_image_memory = vk_image_memory;

    return image;
}

void ValImage::release(ValInstance *p_val_instance) {
    ValReleasable::release(p_val_instance);

    if (vk_image != nullptr) {
        vkDestroyImage(p_val_instance->vk_device, vk_image, nullptr);
        vk_image = nullptr;
    }

    if (vk_image_memory != nullptr) {
        vkFreeMemory(p_val_instance->vk_device, vk_image_memory, nullptr);
        vk_image_memory = nullptr;
    }

    if (vk_image_view != nullptr) {
        vkDestroyImageView(p_val_instance->vk_device, vk_image_view, nullptr);
        vk_image_view = nullptr;
    }
}