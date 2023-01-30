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

    // TODO: Move this to vma?
    VkMemoryRequirements mem_requirements {};
    vkGetImageMemoryRequirements(p_val_instance->vk_device, vk_image, &mem_requirements);

    VmaAllocationCreateInfo vma_alloc_info {};

    VmaAllocation vma_allocation = nullptr;
    VmaAllocationInfo vma_allocation_info;
    vmaAllocateMemoryForImage(p_val_instance->vma_allocator, vk_image, &vma_alloc_info, &vma_allocation, &vma_allocation_info);

    vmaBindImageMemory(p_val_instance->vma_allocator, vma_allocation, vk_image);

    VkImageViewCreateInfo image_view_info{};
    image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    image_view_info.image = vk_image;
    image_view_info.viewType = view_type;
    image_view_info.format = p_create_info->format;

    image_view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    image_view_info.subresourceRange.aspectMask = p_create_info->aspect_flags;
    image_view_info.subresourceRange.baseMipLevel = 0;
    image_view_info.subresourceRange.levelCount = p_create_info->mip_levels;
    image_view_info.subresourceRange.baseArrayLayer = 0;
    image_view_info.subresourceRange.layerCount = 1;

    VkImageView vk_image_view = nullptr;
    if (vkCreateImageView(p_val_instance->vk_device, &image_view_info, nullptr, &vk_image_view) != VK_SUCCESS) {
        vkDestroyImage(p_val_instance->vk_device, vk_image, nullptr);
        vmaFreeMemory(p_val_instance->vma_allocator, vma_allocation);

        return nullptr;
    }

    VkSampler vk_sampler = nullptr;
    if (p_create_info->create_sampler) {
        ValSamplerCreateInfo sampler_info = p_create_info->sampler_info;

        VkSamplerCreateInfo sampler_create_info{};
        sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_create_info.magFilter = sampler_info.mag_filter;
        sampler_create_info.minFilter = sampler_info.min_filter;

        sampler_create_info.addressModeU = sampler_info.mode_u;
        sampler_create_info.addressModeV = sampler_info.mode_v;
        sampler_create_info.addressModeW = sampler_info.mode_w;

        sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_create_info.unnormalizedCoordinates = VK_FALSE;
        sampler_create_info.compareEnable = VK_FALSE;
        sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;

        // TODO: Enable Aniso if found and set the max
        sampler_create_info.anisotropyEnable = VK_FALSE;
        sampler_create_info.maxAnisotropy = 1.0F;

        sampler_create_info.mipmapMode = sampler_info.mip_mode;
        sampler_create_info.mipLodBias = 0.0f;
        sampler_create_info.minLod = 0.0f;
        sampler_create_info.maxLod = 0.0f;

        if (vkCreateSampler(p_val_instance->vk_device, &sampler_create_info, nullptr, &vk_sampler) != VK_SUCCESS) {
            // TODO: Cleanup
            return nullptr;
        }
    }

    ValImage* image = new ValImage();
    image->vk_image = vk_image;
    image->vk_image_view = vk_image_view;
    image->vk_sampler = vk_sampler;
    image->vma_allocation = vma_allocation;
    image->vk_extent = p_create_info->extent;
    image->vk_aspect_flags = p_create_info->aspect_flags;

    return image;
}

void ValImage::write_bytes(unsigned char *bytes, VkCommandBuffer vk_command_buffer, ValInstance *p_val_instance) {
    // We need to allocate a staging buffer
    // TODO: Don't always expect RGBA?
    val_buffer = new ValBuffer(
            vk_extent.width * vk_extent.height * 4,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            p_val_instance);

    val_buffer->write(bytes, p_val_instance);

    VkBufferImageCopy copy_region{};
    copy_region.bufferOffset = 0;
    copy_region.bufferRowLength = 0;
    copy_region.bufferImageHeight = 0;

    copy_region.imageSubresource.aspectMask = vk_aspect_flags;
    copy_region.imageSubresource.mipLevel = 0;
    copy_region.imageSubresource.baseArrayLayer = 0;
    copy_region.imageSubresource.layerCount = 1;

    copy_region.imageOffset = {0, 0, 0};
    copy_region.imageExtent = vk_extent;

    transfer_layout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, vk_command_buffer);

    vkCmdCopyBufferToImage(vk_command_buffer,
                           val_buffer->vk_buffer,
                           vk_image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &copy_region);

    transfer_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, vk_command_buffer);
}

void ValImage::transfer_layout(VkImageLayout old_layout, VkImageLayout new_layout, VkCommandBuffer vk_command_buffer) {
    VkImageMemoryBarrier barrier{};

    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = vk_image;
    barrier.subresourceRange.aspectMask = vk_aspect_flags;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags source_stage = 0;
    VkPipelineStageFlags destination_stage = 0;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }

    if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    }

    vkCmdPipelineBarrier(
            vk_command_buffer,
            source_stage,
            destination_stage,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier
    );
}

void ValImage::post_write(ValInstance *p_val_instance) {
    if (val_buffer != nullptr) {
        val_buffer->release(p_val_instance);
        delete val_buffer;
        val_buffer = nullptr;
    }
}

void ValImage::release(ValInstance *p_val_instance) {
    ValReleasable::release(p_val_instance);

    if (vk_image != nullptr) {
        vkDestroyImage(p_val_instance->vk_device, vk_image, nullptr);
        vk_image = nullptr;
    }

    if (vk_sampler != nullptr) {
        vkDestroySampler(p_val_instance->vk_device, vk_sampler, nullptr);
        vk_sampler = nullptr;
    }

    if (vk_image_view != nullptr) {
        vkDestroyImageView(p_val_instance->vk_device, vk_image_view, nullptr);
        vk_image_view = nullptr;
    }

    if (vma_allocation != nullptr) {
        vmaFreeMemory(p_val_instance->vma_allocator, vma_allocation);
        vma_allocation = nullptr;
    }
}