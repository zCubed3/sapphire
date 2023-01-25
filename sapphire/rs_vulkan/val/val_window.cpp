#include "val_window.h"

#include <SDL.h>
#include <SDL_vulkan.h>

#include <rs_vulkan/val/val_instance.h>

ValWindow::ValWindow(SDL_Window *p_window, VkInstance vk_instance) {
    SDL_Vulkan_CreateSurface(p_window, vk_instance, &vk_surface);
    sdl_window = p_window;
}

ValWindow::PresentInfo* ValWindow::get_present_info(VkPhysicalDevice vk_gpu) const {
    ValWindow::PresentInfo* present_info = new ValWindow::PresentInfo();

    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;

    uint32_t enumeration_count;

    vkGetPhysicalDeviceSurfaceFormatsKHR(vk_gpu, vk_surface, &enumeration_count, nullptr);
    formats.resize(enumeration_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vk_gpu, vk_surface, &enumeration_count, formats.data());

    if (enumeration_count == 0) {
        // TODO: Error no formats available!
        delete present_info;
        return nullptr;
    }

    enumeration_count = 0;

    vkGetPhysicalDeviceSurfacePresentModesKHR(vk_gpu, vk_surface, &enumeration_count, nullptr);
    present_modes.resize(enumeration_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(vk_gpu, vk_surface, &enumeration_count, present_modes.data());

    if (enumeration_count == 0) {
        // TODO: Error no present modes available!
        delete present_info;
        return nullptr;
    }

    enumeration_count = 0;

    // TODO: Smarter swapchain format picking
    // TODO: Linear / sRGB switching?

    for (VkSurfaceFormatKHR format: formats) {
        /*
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
            vk_chosen_format = format;
            break;
        }
         */

        // TODO: User configurable depth format
        if (format.format == VK_FORMAT_D32_SFLOAT) {
            present_info->vk_depth_format = format;
            break;
        }

        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
            present_info->vk_color_format = format;
            break;
        }
    }

    // TODO: Better presentation mode decision
    for (VkPresentModeKHR present_mode: present_modes) {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR || present_mode == VK_PRESENT_MODE_FIFO_KHR) {
            present_info->vk_mode = present_mode;
            break;
        }
    }

    return present_info;
}

bool ValWindow::recreate_swapchain(ValInstance* p_val_instance) {
    p_val_instance->await_frame();

    if (vk_depth_image != nullptr) {
        vkDestroyImage(p_val_instance->vk_device, vk_depth_image, nullptr);
    }

    if (vk_depth_image_view != nullptr) {
        vkDestroyImageView(p_val_instance->vk_device, vk_depth_image_view, nullptr);
    }

    // TODO: Move extent calculation elsewhere
    // TODO: SDL optional dependency
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p_val_instance->vk_physical_device, vk_surface, &vk_capabilities);

    //
    // Hi-dpi aware extent
    //
    if (vk_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        vk_extent = vk_capabilities.currentExtent;
    } else {
        int width, height;
        SDL_Vulkan_GetDrawableSize(sdl_window, &width, &height);

        VkExtent2D actual_extent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };

        actual_extent.width = std::min(std::max(actual_extent.width, vk_capabilities.minImageExtent.width), vk_capabilities.maxImageExtent.width);
        actual_extent.height = std::min(std::max(actual_extent.height, vk_capabilities.minImageExtent.height), vk_capabilities.maxImageExtent.height);

        vk_extent = actual_extent;
    }

    //
    // Swapchain
    //
    uint32_t image_count = vk_capabilities.minImageCount + 1;

    ValQueue graphics_queue = p_val_instance->get_queue(ValQueue::QueueType::QUEUE_TYPE_GRAPHICS);
    ValQueue present_queue = p_val_instance->get_queue(ValQueue::QueueType::QUEUE_TYPE_PRESENT);

    std::vector<uint32_t> device_queues {graphics_queue.family, present_queue.family};

    if (vk_capabilities.maxImageCount > 0 && image_count > vk_capabilities.maxImageCount) {
        image_count = vk_capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchain_create_info{};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    swapchain_create_info.surface = vk_surface;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = p_val_instance->present_info->vk_color_format.format;
    swapchain_create_info.imageColorSpace = p_val_instance->present_info->vk_color_format.colorSpace;
    swapchain_create_info.imageExtent = vk_extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    swapchain_create_info.preTransform = vk_capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    swapchain_create_info.presentMode = p_val_instance->present_info->vk_mode;
    swapchain_create_info.clipped = VK_TRUE;

    swapchain_create_info.oldSwapchain = vk_swapchain;

    if (graphics_queue.family != present_queue.family) {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = static_cast<uint32_t>(device_queues.size());
        swapchain_create_info.pQueueFamilyIndices = device_queues.data();
    } else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0;
        swapchain_create_info.pQueueFamilyIndices = nullptr;
    }

    if (vkCreateSwapchainKHR(p_val_instance->vk_device, &swapchain_create_info, nullptr, &vk_swapchain) != VK_SUCCESS) {
        // TODO: Error failed to create swapchain!
        return false;
    }

    //
    // Swapchain image views
    //

    // Clean up existing views
    for (VkImageView view : vk_swapchain_image_views) {
        vkDestroyImageView(p_val_instance->vk_device, view, nullptr);
    }

    // Clean up existing framebuffers
    for (VkFramebuffer framebuffer : vk_swapchain_framebuffers) {
        vkDestroyFramebuffer(p_val_instance->vk_device, framebuffer, nullptr);
    }

    vkGetSwapchainImagesKHR(p_val_instance->vk_device, vk_swapchain, &image_count, nullptr);
    vk_swapchain_images.resize(image_count);
    vkGetSwapchainImagesKHR(p_val_instance->vk_device, vk_swapchain, &image_count, vk_swapchain_images.data());

    vk_swapchain_image_views.resize(image_count);
    for (uint32_t i = 0; i < image_count; i++) {
        VkImageViewCreateInfo view_create_info{};
        view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

        view_create_info.image = vk_swapchain_images[i];
        view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_create_info.format = p_val_instance->present_info->vk_color_format.format;

        view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_create_info.subresourceRange.baseMipLevel = 0;
        view_create_info.subresourceRange.levelCount = 1;
        view_create_info.subresourceRange.baseArrayLayer = 0;
        view_create_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(p_val_instance->vk_device, &view_create_info, nullptr, &vk_swapchain_image_views[i]) != VK_SUCCESS) {
            // TODO: Error failed to create image view!
            return false;
        }
    }

    // TODO: TEMP
    // Depth buffer
    // TODO: TEMP
    VkImageCreateInfo depth_create_info {};
    depth_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    depth_create_info.imageType = VK_IMAGE_TYPE_2D;
    depth_create_info.extent.width = vk_extent.width;
    depth_create_info.extent.height = vk_extent.height;
    depth_create_info.extent.depth = 1;
    depth_create_info.mipLevels = 1;
    depth_create_info.arrayLayers = 1;
    depth_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_create_info.format = VK_FORMAT_D32_SFLOAT;
    depth_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    depth_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depth_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(p_val_instance->vk_device, &depth_create_info, nullptr, &vk_depth_image) != VK_SUCCESS) {
        return false;
    }

    VkMemoryRequirements mem_requirements {};
    vkGetImageMemoryRequirements(p_val_instance->vk_device, vk_depth_image, &mem_requirements);

    VmaAllocationCreateInfo depth_info {};
    uint32_t memory_type;
    vmaFindMemoryTypeIndex(p_val_instance->vma_allocator, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &depth_info, &memory_type);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = mem_requirements.size;
    allocInfo.memoryTypeIndex = memory_type;

    if (vkAllocateMemory(p_val_instance->vk_device, &allocInfo, nullptr, &vk_depth_image_memory) != VK_SUCCESS) {
        return false;
    }

    vkBindImageMemory(p_val_instance->vk_device, vk_depth_image, vk_depth_image_memory, 0);

    VkImageViewCreateInfo depth_view_create_info{};
    depth_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    depth_view_create_info.image = vk_depth_image;
    depth_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depth_view_create_info.format = VK_FORMAT_D32_SFLOAT;

    depth_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    depth_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    depth_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    depth_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    depth_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    depth_view_create_info.subresourceRange.baseMipLevel = 0;
    depth_view_create_info.subresourceRange.levelCount = 1;
    depth_view_create_info.subresourceRange.baseArrayLayer = 0;
    depth_view_create_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(p_val_instance->vk_device, &depth_view_create_info, nullptr, &vk_depth_image_view) != VK_SUCCESS) {
        // TODO: Error failed to create image view!
        return false;
    }

    vk_swapchain_framebuffers.resize(image_count);
    for (uint32_t i = 0; i < image_count; i++) {
        std::vector<VkImageView> attachments = {
                vk_swapchain_image_views[i],
                vk_depth_image_view
        };

        VkFramebufferCreateInfo framebuffer_create_info{};
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.renderPass = p_val_instance->vk_render_pass;
        framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebuffer_create_info.pAttachments = attachments.data();
        framebuffer_create_info.width = vk_extent.width;
        framebuffer_create_info.height = vk_extent.height;
        framebuffer_create_info.layers = 1;

        if (vkCreateFramebuffer(p_val_instance->vk_device, &framebuffer_create_info, nullptr, &vk_swapchain_framebuffers[i]) != VK_SUCCESS) {
            // TODO: Error, failed to create framebuffer!
            return false;
        }
    }

    return true;
}

bool ValWindow::begin_rendering(ValInstance* p_val_instance) {
    if (vk_command_buffer == nullptr) {
        vk_command_buffer = p_val_instance->get_queue(ValQueue::QUEUE_TYPE_GRAPHICS).allocate_buffer(p_val_instance);
    }

    vkAcquireNextImageKHR(
            p_val_instance->vk_device,
            vk_swapchain,
            UINT64_MAX,
            p_val_instance->vk_image_available_semaphore,
            VK_NULL_HANDLE,
            &vk_frame_index);

    VkCommandBufferBeginInfo buffer_begin_info{};
    buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    buffer_begin_info.flags = 0;
    buffer_begin_info.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(vk_command_buffer, &buffer_begin_info) != VK_SUCCESS) {
        // TODO: Failed to record buffer
        return false;
    }

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = p_val_instance->vk_render_pass;
    render_pass_info.framebuffer = vk_swapchain_framebuffers[vk_frame_index];

    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = vk_extent;

    std::vector<VkClearValue> clear_values;

    VkClearValue clear_color = {};
    clear_color.color = { 0, 0, 0, 1 };

    VkClearValue depth_stencil = {};
    depth_stencil.depthStencil = {1.0F, 0};

    clear_values.push_back(clear_color);
    clear_values.push_back(depth_stencil);

    render_pass_info.clearValueCount = clear_values.size();
    render_pass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(vk_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    return true;
}

bool ValWindow::end_rendering(ValInstance* p_val_instance) {
    vkCmdEndRenderPass(vk_command_buffer);
    vkEndCommandBuffer(vk_command_buffer);

    return true;
}

bool ValWindow::present_queue(ValInstance *p_val_instance) {
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &p_val_instance->vk_image_available_semaphore;
    submit_info.pWaitDstStageMask = wait_stages;

    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &p_val_instance->vk_render_finished_semaphore;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &vk_command_buffer;

    if (vkQueueSubmit(p_val_instance->get_queue(ValQueue::QUEUE_TYPE_GRAPHICS).vk_queue, 1, &submit_info, p_val_instance->vk_render_fence) != VK_SUCCESS) {
        return false;
    }

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &p_val_instance->vk_render_finished_semaphore;

    present_info.swapchainCount = 1;
    present_info.pSwapchains = &vk_swapchain;
    present_info.pImageIndices = &vk_frame_index;

    vkQueuePresentKHR(p_val_instance->get_queue(ValQueue::QUEUE_TYPE_PRESENT).vk_queue, &present_info);

    return true;
}

void ValWindow::release(ValWindow *p_window, ValInstance *p_val_instance) {
    for (VkImageView view : p_window->vk_swapchain_image_views) {
        vkDestroyImageView(p_val_instance->vk_device, view, nullptr);
    }

    for (VkFramebuffer framebuffer : p_window->vk_swapchain_framebuffers) {
        vkDestroyFramebuffer(p_val_instance->vk_device, framebuffer, nullptr);
    }

    vkDestroySwapchainKHR(p_val_instance->vk_device, p_window->vk_swapchain, nullptr);
    vkDestroySurfaceKHR(p_val_instance->vk_instance, p_window->vk_surface, nullptr);
}