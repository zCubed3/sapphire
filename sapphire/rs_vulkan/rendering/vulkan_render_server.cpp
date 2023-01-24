#include "vulkan_render_server.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <engine/assets/mesh_asset.h>
#include <engine/rendering/render_target.h>

#include <rs_vulkan/assets/vulkan_shader_asset.h>
#include <rs_vulkan/assets/vulkan_shader_asset_loader.h>

#include <rs_vulkan/rendering/vulkan_mesh_buffer.h>

#define RS_VULKAN_DEBUG

// Rather than making the glove fit the hand; The hand will fit the glove
// Eg. Our engine will fit Vulkan, we won't try to make Vulkan fit us

// TODO: Move this into Vulkan render target code
bool VulkanRenderServer::recreate_swapchain() {
    await_frame();

    // TODO: Move extent calculation elsewhere
    // TODO: SDL optional dependency
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_physical_device, vk_surface, &vk_capabilities);

    if (vk_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        vk_extent = vk_capabilities.currentExtent;
    } else {
        int width, height;
        SDL_Vulkan_GetDrawableSize(window, &width, &height);

        VkExtent2D actual_extent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };

        actual_extent.width = std::min(std::max(actual_extent.width, vk_capabilities.minImageExtent.width), vk_capabilities.maxImageExtent.width);
        actual_extent.height = std::min(std::max(actual_extent.height, vk_capabilities.minImageExtent.height), vk_capabilities.maxImageExtent.height);

        vk_extent = actual_extent;
    }

    uint32_t image_count = vk_capabilities.minImageCount + 1;
    std::vector<uint32_t> device_queues = {vk_graphics_family, vk_present_family};

    if (vk_capabilities.maxImageCount > 0 && image_count > vk_capabilities.maxImageCount) {
        image_count = vk_capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchain_create_info{};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

    swapchain_create_info.surface = vk_surface;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = vk_chosen_format.format;
    swapchain_create_info.imageColorSpace = vk_chosen_format.colorSpace;
    swapchain_create_info.imageExtent = vk_extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    swapchain_create_info.preTransform = vk_capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    swapchain_create_info.presentMode = vk_chosen_present_mode;
    swapchain_create_info.clipped = VK_TRUE;

    swapchain_create_info.oldSwapchain = vk_swapchain;

    if (vk_graphics_family != vk_present_family) {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = device_queues.size();
        swapchain_create_info.pQueueFamilyIndices = device_queues.data();
    } else {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0;
        swapchain_create_info.pQueueFamilyIndices = nullptr;
    }

    if (vkCreateSwapchainKHR(vk_device, &swapchain_create_info, nullptr, &vk_swapchain) != VK_SUCCESS) {
        // TODO: Error failed to create swapchain!
        return false;
    }

    //
    // Swapchain image views
    //

    // Clean up existing views
    for (VkImageView view : vk_swapchain_image_views) {
        vkDestroyImageView(vk_device, view, nullptr);
    }

    // Clean up existing framebuffers
    for (VkFramebuffer framebuffer : vk_swapchain_framebuffers) {
        vkDestroyFramebuffer(vk_device, framebuffer, nullptr);
    }

    vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &image_count, nullptr);
    vk_swapchain_images.resize(image_count);
    vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &image_count, vk_swapchain_images.data());

    vk_swapchain_image_views.resize(image_count);
    for (int i = 0; i < image_count; i++) {
        VkImageViewCreateInfo view_create_info{};
        view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

        view_create_info.image = vk_swapchain_images[i];
        view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_create_info.format = vk_chosen_format.format;

        view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_create_info.subresourceRange.baseMipLevel = 0;
        view_create_info.subresourceRange.levelCount = 1;
        view_create_info.subresourceRange.baseArrayLayer = 0;
        view_create_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(vk_device, &view_create_info, nullptr, &vk_swapchain_image_views[i]) != VK_SUCCESS) {
            // TODO: Error failed to create image view!
            return false;
        }
    }

    vk_swapchain_framebuffers.resize(image_count);
    for (int i = 0; i < image_count; i++) {
        VkImageView attachments[] = {
            vk_swapchain_image_views[i]
        };

        VkFramebufferCreateInfo framebuffer_create_info{};
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.renderPass = vk_render_pass;
        framebuffer_create_info.attachmentCount = 1;
        framebuffer_create_info.pAttachments = attachments;
        framebuffer_create_info.width = vk_extent.width;
        framebuffer_create_info.height = vk_extent.height;
        framebuffer_create_info.layers = 1;

        if (vkCreateFramebuffer(vk_device, &framebuffer_create_info, nullptr, &vk_swapchain_framebuffers[i]) != VK_SUCCESS) {
            // TODO: Error, failed to create framebuffer!
            return false;
        }
    }

    return true;
}

void VulkanRenderServer::await_frame() {
    if (vk_device != nullptr && vk_flight_fence != nullptr) {
        vkWaitForFences(vk_device, 1, &vk_flight_fence, VK_TRUE, UINT64_MAX);
    }
}

// TODO: Wait for rendering to finish
VulkanRenderServer::~VulkanRenderServer() {
    await_frame();

    if (vk_swapchain != nullptr) {
        vkDestroySwapchainKHR(vk_device, vk_swapchain, nullptr);
    }

    for (VkImageView view : vk_swapchain_image_views) {
        vkDestroyImageView(vk_device, view, nullptr);
    }

    for (VkFramebuffer framebuffer : vk_swapchain_framebuffers) {
        vkDestroyFramebuffer(vk_device, framebuffer, nullptr);
    }

    if (vk_render_pass != nullptr) {
        vkDestroyRenderPass(vk_device, vk_render_pass, nullptr);
    }

    if (vk_image_available_semaphore != nullptr) {
        vkDestroySemaphore(vk_device, vk_image_available_semaphore, nullptr);
    }

    if (vk_render_finished_semaphore != nullptr) {
        vkDestroySemaphore(vk_device, vk_render_finished_semaphore, nullptr);
    }

    if (vk_flight_fence != nullptr) {
        vkDestroyFence(vk_device, vk_flight_fence, nullptr);
    }

    if (vk_active_command_buffer != nullptr) {
        vkFreeCommandBuffers(vk_device, vk_command_pool, 1, &vk_active_command_buffer);
    }

    if (vk_active_command_buffer != nullptr) {
        vkDestroyCommandPool(vk_device, vk_command_pool, nullptr);
    }

    if (vk_device != nullptr) {
        vkDestroyDevice(vk_device, nullptr);
    }

    if (vk_surface != nullptr) {
        vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);
    }

    if (vk_instance != nullptr) {
        vkDestroyInstance(vk_instance, nullptr);
    }
}

void VulkanRenderServer::register_rs_asset_loaders() {
    AssetLoader::register_loader<VulkanShaderAssetLoader>();
}

std::string VulkanRenderServer::get_name() const {
    // TODO: Include API version?
    return "Vulkan";
}

std::string VulkanRenderServer::get_error() const {
    // TODO: Vulkan render server errors
    return "";
}

uint32_t VulkanRenderServer::get_sdl_window_flags() const {
    return SDL_WINDOW_VULKAN;
}

// TODO: Vulkan render server errors
bool VulkanRenderServer::initialize(SDL_Window *p_window) {
    if (singleton != nullptr) {
        return false;
    }

    if (p_window == nullptr) {
        return false;
    }

    window = p_window;

    // Vulkan bootstrapping is so fun!

    //
    // VkInstance creation
    //
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Sapphire";// TODO: Separate application name?
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Sapphire";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;// TODO: Changing API versions?

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    // TODO: Move extension code somewhere else
    // TODO: Allow loading other extensions?
    std::vector<const char *> enabled_instance_extensions;
    std::vector<const char *> enabled_layers;

    uint32_t enumeration_count;

    std::vector<VkExtensionProperties> user_instance_extensions;
    std::vector<VkLayerProperties> user_layers;

    vkEnumerateInstanceExtensionProperties(nullptr, &enumeration_count, nullptr);
    user_instance_extensions.resize(enumeration_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &enumeration_count, user_instance_extensions.data());

    enumeration_count = 0;

    vkEnumerateInstanceLayerProperties(&enumeration_count, nullptr);
    user_layers.resize(enumeration_count);
    vkEnumerateInstanceLayerProperties(&enumeration_count, user_layers.data());

    enumeration_count = 0;

    SDL_Vulkan_GetInstanceExtensions(p_window, &enumeration_count, nullptr);
    enabled_instance_extensions.resize(enumeration_count);
    SDL_Vulkan_GetInstanceExtensions(p_window, &enumeration_count, enabled_instance_extensions.data());

    enumeration_count = 0;

    // We need to ensure the extensions and layers we require do exist
    std::vector<const char *> missing_extensions;
    for (const char *extension: enabled_instance_extensions) {
        bool has_extension = false;
        for (VkExtensionProperties properties: user_instance_extensions) {
            if (strcmp(extension, properties.extensionName) == 0) {
                has_extension = true;
                break;
            }
        }

        if (!has_extension) {
            missing_extensions.push_back(extension);
        }
    }

    std::vector<const char *> missing_layers;
    for (const char *layer: enabled_layers) {
        bool has_layer = false;
        for (VkLayerProperties properties: user_layers) {
            if (strcmp(layer, properties.layerName) == 0) {
                has_layer = true;
                break;
            }
        }

        if (!has_layer) {
            missing_extensions.push_back(layer);
        }
    }

    // TODO: Present this better than logging it to cout
    // TODO: Optional extensions and layers?
    if (!missing_extensions.empty()) {
        std::cout << "Missing Instance Extension(s):" << std::endl;
        for (const char *missing: missing_extensions) {
            std::cout << '\t' << missing << std::endl;
        }

        return false;
    }

    if (!missing_layers.empty()) {
        std::cout << "Missing Layer(s):" << std::endl;
        for (const char *missing: missing_layers) {
            std::cout << '\t' << missing << std::endl;
        }

        return false;
    }


#ifdef RS_VULKAN_DEBUG
    std::cout << "Found Instance Extensions:" << std::endl;
    for (VkExtensionProperties properties: user_instance_extensions) {
        std::cout << '\t' << properties.extensionName << std::endl;
    }

    std::cout << "Found Layers:" << std::endl;
    for (VkLayerProperties properties: user_layers) {
        std::cout << '\t' << properties.layerName << std::endl;
    }

    std::cout << std::endl;

    // If we're debugging we need validation layers :P
    enabled_layers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif

    create_info.enabledExtensionCount = enabled_instance_extensions.size();
    create_info.ppEnabledExtensionNames = enabled_instance_extensions.data();

    create_info.enabledLayerCount = enabled_layers.size();
    create_info.ppEnabledLayerNames = enabled_layers.data();

#ifdef RS_VULKAN_DEBUG
    std::cout << "Enabled Instance Extensions:" << std::endl;
    for (const char *extension: enabled_instance_extensions) {
        std::cout << '\t' << extension << std::endl;
    }

    std::cout << "Enabled Layers:" << std::endl;
    for (const char *layer: enabled_layers) {
        std::cout << '\t' << layer << std::endl;
    }

    std::cout << std::endl;
#endif

    if (vkCreateInstance(&create_info, nullptr, &vk_instance) != VK_SUCCESS) {
        return false;
    }

    //
    // Surface
    //
    if (!SDL_Vulkan_CreateSurface(p_window, vk_instance, &vk_surface)) {
        return false;
    }

    //
    // Physical devices
    //
    std::vector<VkPhysicalDevice> user_physical_devices;

    vkEnumeratePhysicalDevices(vk_instance, &enumeration_count, nullptr);
    user_physical_devices.resize(enumeration_count);
    vkEnumeratePhysicalDevices(vk_instance, &enumeration_count, user_physical_devices.data());

    if (enumeration_count == 0) {
        // TODO: Error there are no vulkan supported GPUs!
        return false;
    }

    enumeration_count = 0;

#ifdef RS_VULKAN_DEBUG
    std::cout << "Found GPUs:" << std::endl;
    uint32_t gpu_index = 0;
#endif

    for (VkPhysicalDevice device: user_physical_devices) {
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        std::vector<VkQueueFamilyProperties> queue_families;

        vkGetPhysicalDeviceProperties(device, &properties);
        vkGetPhysicalDeviceFeatures(device, &features);

        vkGetPhysicalDeviceQueueFamilyProperties(device, &enumeration_count, nullptr);
        queue_families.resize(enumeration_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &enumeration_count, queue_families.data());

        enumeration_count = 0;

        uint32_t graphics_family_index = -1;
        uint32_t present_family_index = -1;

        uint32_t index = 0;
        for (VkQueueFamilyProperties queue_family: queue_families) {
            if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphics_family_index = index;
            }

            VkBool32 surface_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, index, vk_surface, &surface_support);
            if (surface_support) {
                present_family_index = index;
            }

            index++;
        }

        if (graphics_family_index == -1 && present_family_index == -1) {
            // TODO: Log this GPU as incompatible
            continue;
        }

        // TODO: Optional required features
        // TODO: GPU ranking / picking
        // TODO: Config option to manually decide GPU?

#ifdef RS_VULKAN_DEBUG
        std::cout << "\t"
                  << "GPU" << gpu_index << ": " << properties.deviceName << std::endl;
        std::cout << "\t\t"
                  << "Graphics Queue = " << graphics_family_index << std::endl;
        std::cout << "\t\t"
                  << "Present Queue = " << present_family_index << std::endl;
#endif

        // TODO: Go through ALL GPUs and don't pick GPU0 as the first always!
        vk_physical_device = device;
        vk_graphics_family = graphics_family_index;
        vk_present_family = present_family_index;

        gpu_index++;
        break;
    }

#ifdef RS_VULKAN_DEBUG
    std::cout << std::endl;
#endif

    //
    // Logical device
    //
    std::vector<const char*> enabled_device_extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    std::vector<VkExtensionProperties> user_device_extensions;

    std::vector<uint32_t> device_queues = {vk_graphics_family, vk_present_family};
    std::vector<VkDeviceQueueCreateInfo> device_queue_infos;

    float queue_priority = 1;

    for (uint32_t queue: device_queues) {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;

        device_queue_infos.emplace_back(queue_create_info);
    }

    vkEnumerateDeviceExtensionProperties(vk_physical_device, nullptr, &enumeration_count, nullptr);
    user_device_extensions.resize(enumeration_count);
    vkEnumerateDeviceExtensionProperties(vk_physical_device, nullptr, &enumeration_count, user_device_extensions.data());

    enumeration_count = 0;

    std::vector<const char *> missing_device_extensions;
    for (const char *extension: enabled_device_extensions) {
        bool has_extension = false;
        for (VkExtensionProperties properties: user_device_extensions) {
            if (strcmp(extension, properties.extensionName) == 0) {
                has_extension = true;
                break;
            }
        }

        if (!has_extension) {
            missing_device_extensions.push_back(extension);
        }
    }

    if (!missing_device_extensions.empty()) {
        std::cout << "Missing Device Extension(s):" << std::endl;
        for (const char *missing: missing_device_extensions) {
            std::cout << '\t' << missing << std::endl;
        }

        return false;
    }


#ifdef RS_VULKAN_DEBUG
    std::cout << "Found Device Extensions:" << std::endl;
    for (VkExtensionProperties properties: user_device_extensions) {
        std::cout << '\t' << properties.extensionName << std::endl;
    }

    std::cout << "Enabled Device Extensions:" << std::endl;
    for (const char* extension: enabled_device_extensions) {
        std::cout << '\t' << extension << std::endl;
    }

    std::cout << std::endl;
#endif

    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    device_create_info.pQueueCreateInfos = device_queue_infos.data();
    device_create_info.queueCreateInfoCount = device_queue_infos.size();

    device_create_info.ppEnabledExtensionNames = enabled_device_extensions.data();
    device_create_info.enabledExtensionCount = enabled_device_extensions.size();

    device_create_info.enabledLayerCount = 0;

    if (vkCreateDevice(vk_physical_device, &device_create_info, nullptr, &vk_device) != VK_SUCCESS) {
        return false;
    }

    vkGetDeviceQueue(vk_device, vk_graphics_family, 0, &vk_graphics_queue);
    vkGetDeviceQueue(vk_device, vk_present_family, 0, &vk_present_queue);

    //
    // Format decision
    //
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_physical_device, vk_surface, &vk_capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical_device, vk_surface, &enumeration_count, nullptr);
    formats.resize(enumeration_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical_device, vk_surface, &enumeration_count, formats.data());

    if (enumeration_count == 0) {
        // TODO: Error no formats available!
        return false;
    }

    enumeration_count = 0;

    vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physical_device, vk_surface, &enumeration_count, nullptr);
    present_modes.resize(enumeration_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physical_device, vk_surface, &enumeration_count, present_modes.data());

    if (enumeration_count == 0) {
        // TODO: Error no present modes available!
        return false;
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

        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
            vk_chosen_format = format;
            break;
        }
    }

    // TODO: Non-vsync present mode (FIFO)
    for (VkPresentModeKHR present_mode: present_modes) {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR || present_mode == VK_PRESENT_MODE_FIFO_KHR) {
            vk_chosen_present_mode = present_mode;
            break;
        }
    }

    //
    // Render pass
    //
    VkAttachmentDescription color_attachment{};
    color_attachment.format = vk_chosen_format.format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkRenderPassCreateInfo render_pass_create_info{};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &color_attachment;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass;

    if (vkCreateRenderPass(vk_device, &render_pass_create_info, nullptr, &vk_render_pass) != VK_SUCCESS) {
        // TODO: Error failed to create render pass
        return false;
    }

    //
    // Swapchain
    //
    recreate_swapchain();

    //
    // Command pool
    //
    VkCommandPoolCreateInfo pool_create_info{};
    pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

    pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_create_info.queueFamilyIndex = vk_graphics_family;

    if (vkCreateCommandPool(vk_device, &pool_create_info, nullptr, &vk_command_pool) != VK_SUCCESS) {
        // TODO: Error failed to create command pool
        return false;
    }

    VkCommandBufferAllocateInfo command_buffer_alloc_info{};
    command_buffer_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_alloc_info.commandPool = vk_command_pool;
    command_buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_alloc_info.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(vk_device, &command_buffer_alloc_info, &vk_active_command_buffer) != VK_SUCCESS) {
        // TODO: Error, failed top create command buffer
        return false;
    }

    //
    // Sync objects
    //
    VkSemaphoreCreateInfo semaphore_create_info{};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_create_info{};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    // TODO: Sync object errors
    if (vkCreateSemaphore(vk_device, &semaphore_create_info, nullptr, &vk_image_available_semaphore) != VK_SUCCESS) {
        return false;
    }

    if (vkCreateSemaphore(vk_device, &semaphore_create_info, nullptr, &vk_render_finished_semaphore) != VK_SUCCESS) {
        return false;
    }

    if (vkCreateFence(vk_device, &fence_create_info, nullptr, &vk_flight_fence) != VK_SUCCESS) {
        return false;
    }

    singleton = this;

    return true;
}

bool VulkanRenderServer::present(SDL_Window *p_window) {
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {vk_image_available_semaphore};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;

    VkSemaphore signal_semaphores[] = {vk_render_finished_semaphore};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &vk_active_command_buffer;

    if (vkQueueSubmit(vk_graphics_queue, 1, &submit_info, vk_flight_fence) != VK_SUCCESS) {
        // TODO: Error failed to submit buffer
        return false;
    }

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    present_info.swapchainCount = 1;
    present_info.pSwapchains = &vk_swapchain;
    present_info.pImageIndices = &vk_frame_index;

    vkQueuePresentKHR(vk_present_queue, &present_info);

    return true;
}

void VulkanRenderServer::on_window_resized() {
    recreate_swapchain();
}

bool VulkanRenderServer::begin_frame() {
    await_frame();
    vkResetFences(vk_device, 1, &vk_flight_fence);

    vkAcquireNextImageKHR(
            vk_device,
            vk_swapchain,
            UINT64_MAX,
            vk_image_available_semaphore,
            VK_NULL_HANDLE,
            &vk_frame_index);

    return true;
}

bool VulkanRenderServer::end_frame() {
    return true;
}

bool VulkanRenderServer::begin_target(RenderTarget *p_target) {
    VkCommandBufferBeginInfo buffer_begin_info{};
    buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    buffer_begin_info.flags = 0;
    buffer_begin_info.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(vk_active_command_buffer, &buffer_begin_info) != VK_SUCCESS) {
        // TODO: Failed to record buffer
        return false;
    }

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = vk_render_pass;
    render_pass_info.framebuffer = vk_swapchain_framebuffers[vk_frame_index];

    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = vk_extent;

    VkClearValue clear_color = {};
    clear_color.color = { p_target->clear_color[0], p_target->clear_color[1], p_target->clear_color[2], p_target->clear_color[3] };

    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(vk_active_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    return true;
}

bool VulkanRenderServer::end_target(RenderTarget *p_target) {
    vkCmdEndRenderPass(vk_active_command_buffer);

    vkEndCommandBuffer(vk_active_command_buffer);

    // TODO: Submit render targets immediately, defer the main window

    return true;
}

void VulkanRenderServer::populate_mesh_buffer(MeshAsset *p_mesh_asset) const {
    if (p_mesh_asset != nullptr) {
        p_mesh_asset->buffer = new VulkanMeshBuffer(p_mesh_asset);
    }
}