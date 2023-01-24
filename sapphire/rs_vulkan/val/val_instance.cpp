#include "val_instance.h"

#ifdef SDL_SUPPORT
#include <SDL.h>
#include <SDL_vulkan.h>
#endif

// TODO: Verbosity

ValInstance::CreationError ValInstance::last_error = ValInstance::ERR_NONE;

std::vector<ValExtension> ValInstance::validate_instance_extensions(ValInstanceCreateInfo* p_create_info) {
    std::vector<VkExtensionProperties> found_extensions;
    uint32_t found_extension_count;

    vkEnumerateInstanceExtensionProperties(nullptr, &found_extension_count, nullptr);
    found_extensions.resize(found_extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &found_extension_count, found_extensions.data());

    std::vector<ValExtension> missing_extensions;
    for (const ValExtension& extension: p_create_info->instance_extensions) {
        bool has_extension = false;
        for (VkExtensionProperties properties: found_extensions) {
            if (strcmp(extension.name, properties.extensionName) == 0) {
                has_extension = true;
                break;
            }
        }

        if (!has_extension) {
            missing_extensions.push_back(extension);
        }
    }

    return missing_extensions;
}

std::vector<ValExtension> ValInstance::validate_device_extensions(VkPhysicalDevice vk_gpu, ValInstanceCreateInfo* p_create_info) {
    std::vector<VkExtensionProperties> found_extensions;
    uint32_t found_extension_count;

    vkEnumerateDeviceExtensionProperties(vk_gpu, nullptr, &found_extension_count, nullptr);
    found_extensions.resize(found_extension_count);
    vkEnumerateDeviceExtensionProperties(vk_gpu, nullptr, &found_extension_count, found_extensions.data());

    std::vector<ValExtension> missing_extensions;
    for (const ValExtension& extension: p_create_info->instance_extensions) {
        bool has_extension = false;
        for (VkExtensionProperties properties: found_extensions) {
            if (strcmp(extension.name, properties.extensionName) == 0) {
                has_extension = true;
                break;
            }
        }

        if (!has_extension) {
            missing_extensions.push_back(extension);
        }
    }

    return missing_extensions;
}

std::vector<ValLayer> ValInstance::validate_layers(ValInstanceCreateInfo* p_create_info) {
    std::vector<VkLayerProperties> found_layers;
    uint32_t found_layer_count;

    vkEnumerateInstanceLayerProperties(&found_layer_count, nullptr);
    found_layers.resize(found_layer_count);
    vkEnumerateInstanceLayerProperties(&found_layer_count, found_layers.data());

    std::vector<ValLayer> missing_layers;
    for (const ValLayer& layer: p_create_info->validation_layers) {
        bool has_extension = false;
        for (VkLayerProperties properties: found_layers) {
            if (strcmp(layer.name, properties.layerName) == 0) {
                has_extension = true;
                break;
            }
        }

        if (!has_extension) {
            missing_layers.push_back(layer);
        }
    }

    return missing_layers;
}

const char* ValInstance::get_error() {
    switch (last_error) {
        default:
            return nullptr;

        case ValInstance::ERR_CREATE_INFO_NULLPTR:
            return "p_create_info was nullpr!";

        case ValInstance::ERR_VK_INSTANCE_FAILURE:
            return "Failed to create VkInstance!";

        case ValInstance::ERR_VK_DEVICE_FAILURE:
            return "Failed to create VkDevice!";

        case ValInstance::ERR_VK_GPU_MISSING:
            return "Failed to find a suitable Vulkan supported GPU!";
    }
}

VkInstance ValInstance::create_vk_instance(ValInstanceCreateInfo *p_create_info) {
    VkInstance vk_instance = nullptr;

    uint32_t application_version = VK_MAKE_VERSION(
            p_create_info->application_major_version,
            p_create_info->application_minor_version,
            p_create_info->application_patch_version);

    uint32_t engine_version = VK_MAKE_VERSION(
            p_create_info->engine_major_version,
            p_create_info->engine_minor_version,
            p_create_info->engine_patch_version);

    uint32_t vulkan_version;

    switch (p_create_info->version) {
        default:
            vulkan_version = VK_API_VERSION_1_0;
            break;

        case ValInstanceCreateInfo::API_VERSION_1_1:
            vulkan_version = VK_API_VERSION_1_1;
            break;

        case ValInstanceCreateInfo::API_VERSION_1_2:
            vulkan_version = VK_API_VERSION_1_2;
            break;

        case ValInstanceCreateInfo::API_VERSION_1_3:
            vulkan_version = VK_API_VERSION_1_3;
            break;
    }

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = p_create_info->application_name;
    app_info.applicationVersion = application_version;
    app_info.pEngineName = p_create_info->engine_name;
    app_info.engineVersion = engine_version;
    app_info.apiVersion = vulkan_version;

    VkInstanceCreateInfo instance_create_info{};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo = &app_info;

    //
    // Extensions
    //
    std::vector<const char*> enabled_extensions {};
    std::vector<ValExtension> missing_extensions = validate_instance_extensions(p_create_info);

    for (const ValExtension& extension: p_create_info->instance_extensions) {
        bool missing_extension = false;
        bool mandatory_extension = false;
        for (const ValExtension &missing: missing_extensions) {
            if (strcmp(extension.name, missing.name) == 0) {
                missing_extension = true;

                if (extension.flags & ValExtension::EXTENSION_FLAG_OPTIONAL) {
                    continue;
                }

                mandatory_extension = true;
            }
        }

        if (!missing_extension) {
            enabled_extensions.push_back(extension.name);
        } else {
            if (mandatory_extension) {
                last_error = ERR_VK_MISSING_INSTANCE_EXTENSION;
                return nullptr;
            }
        }
    }

    instance_create_info.ppEnabledExtensionNames = enabled_extensions.data();
    instance_create_info.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size());

    //
    // Layers
    //
    std::vector<const char*> enabled_layers {};
    std::vector<ValLayer> missing_layers = validate_layers(p_create_info);

    for (const ValLayer& layer: p_create_info->validation_layers) {
        bool missing_layer = false;
        bool mandatory_layer = false;
        for (const ValLayer &missing: missing_layers) {
            if (strcmp(layer.name, missing.name) == 0) {
                missing_layer = true;

                if (layer.flags & ValLayer::LAYER_FLAG_OPTIONAL) {
                    continue;
                }

                mandatory_layer = true;
            }
        }

        if (!missing_layer) {
            enabled_layers.push_back(layer.name);
        } else {
            if (mandatory_layer) {
                last_error = ERR_VK_MISSING_INSTANCE_EXTENSION;
                return nullptr;
            }
        }
    }

    instance_create_info.ppEnabledLayerNames = enabled_layers.data();
    instance_create_info.enabledLayerCount = static_cast<uint32_t>(enabled_layers.size());


    if (vkCreateInstance(&instance_create_info, nullptr, &vk_instance) != VK_SUCCESS) {
        return nullptr;
    }

    return vk_instance;
}

ValInstance::ChosenGPU ValInstance::pick_gpu(VkInstance vk_instance, VkSurfaceKHR vk_surface, ValInstanceCreateInfo *p_create_info) {
    uint32_t gpu_count;
    std::vector<VkPhysicalDevice> gpus;

    vkEnumeratePhysicalDevices(vk_instance, &gpu_count, nullptr);
    gpus.resize(gpu_count);
    vkEnumeratePhysicalDevices(vk_instance, &gpu_count, gpus.data());


    for (VkPhysicalDevice gpu: gpus) {
        uint32_t enumeration_count;

        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        std::vector<VkQueueFamilyProperties> queue_families;

        vkGetPhysicalDeviceProperties(gpu, &properties);
        vkGetPhysicalDeviceFeatures(gpu, &features);

        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &enumeration_count, nullptr);
        queue_families.resize(enumeration_count);
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &enumeration_count, queue_families.data());

        enumeration_count = 0;

        uint32_t queue_index = 0;
        std::vector<ValQueue> queues;

        // TODO: Better queue exclusivity?
        for (VkQueueFamilyProperties queue_family: queue_families) {
            for (ValQueue::QueueType type: p_create_info->requested_queues) {
                // Ensure we haven't already found a suitable queue
                bool already_found = false;

                for (ValQueue queue: queues) {
                    if (queue.type == type) {
                        already_found = true;
                    }
                }

                if (already_found) {
                    continue;
                }

                ValQueue queue {};
                queue.type = type;
                queue.family = queue_index;

                bool unique = false;

                switch (type) {
                    case ValQueue::QUEUE_TYPE_GRAPHICS: {
                        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                            queues.push_back(queue);
                            unique = true;
                        }

                        break;
                    }

                    case ValQueue::QUEUE_TYPE_TRANSFER: {
                        if (queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT) {
                            queues.push_back(queue);
                            unique = true;
                        }

                        break;
                    }

#if SDL_SUPPORT
                    case ValQueue::QUEUE_TYPE_PRESENT: {
                        VkBool32 surface_support = false;
                        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, queue_index, vk_surface, &surface_support);

                        if (surface_support) {
                            queues.push_back(queue);
                            unique = true;
                        }

                        break;
                    }
#endif
                }

                if (unique) {
                    break;
                }
            }

            queue_index++;
        }

        // TODO: Optional required features
        // TODO: GPU ranking / picking
        // TODO: Config option to manually decide GPU?
        // TODO: Go through ALL GPUs and don't pick GPU0 as the default always!

        return ValInstance::ChosenGPU {gpu, queues};
    }

    return ValInstance::ChosenGPU {};
}

VkDevice ValInstance::create_vk_device(ValInstanceCreateInfo* p_create_info, VkPhysicalDevice vk_gpu, std::vector<ValQueue>& val_queues) {
    VkDevice vk_device = nullptr;

    std::vector<uint32_t> device_queues;
    std::vector<VkDeviceQueueCreateInfo> device_queue_infos;

    float queue_priority = 1;

    for (const ValQueue& queue : val_queues) {
        device_queues.push_back(queue.family);

        VkDeviceQueueCreateInfo queue_info {};
        queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.queueFamilyIndex = queue.family;
        queue_info.queueCount = 1;
        queue_info.pQueuePriorities = &queue_priority;

        device_queue_infos.push_back(queue_info);
    }

    for (uint32_t queue: device_queues) {
        VkDeviceQueueCreateInfo queue_create_info{};


    }

    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    device_create_info.pQueueCreateInfos = device_queue_infos.data();
    device_create_info.queueCreateInfoCount = static_cast<uint32_t>(device_queue_infos.size());

    //
    // Extensions
    //
    std::vector<const char*> enabled_extensions {};
    std::vector<ValExtension> missing_extensions = validate_device_extensions(vk_gpu, p_create_info);

    for (const ValExtension& extension: p_create_info->device_extensions) {
        bool missing_extension = false;
        bool mandatory_extension = false;
        for (const ValExtension &missing: missing_extensions) {
            if (strcmp(extension.name, missing.name) == 0) {
                missing_extension = true;

                if (extension.flags & ValExtension::EXTENSION_FLAG_OPTIONAL) {
                    continue;
                }

                mandatory_extension = true;
            }
        }

        if (!missing_extension) {
            enabled_extensions.push_back(extension.name);
        } else {
            if (mandatory_extension) {
                last_error = ERR_VK_MISSING_INSTANCE_EXTENSION;
                return nullptr;
            }
        }
    }

    device_create_info.ppEnabledExtensionNames = enabled_extensions.data();
    device_create_info.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size());

    device_create_info.enabledLayerCount = 0;

    if (vkCreateDevice(vk_gpu, &device_create_info, nullptr, &vk_device) != VK_SUCCESS) {
        last_error = ERR_VK_DEVICE_FAILURE;
        return nullptr;
    }

    for (ValQueue& queue : val_queues) {
        vkGetDeviceQueue(vk_device, queue.family, 0, &queue.vk_queue);
    }

    return vk_device;
}

VkRenderPass ValInstance::create_vk_render_pass(VkDevice vk_device, ValWindow::PresentInfo* present_info) {
    VkAttachmentDescription color_attachment{};
    color_attachment.format = present_info->vk_format.format;
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

    VkRenderPass vk_render_pass;
    if (vkCreateRenderPass(vk_device, &render_pass_create_info, nullptr, &vk_render_pass) != VK_SUCCESS) {
        // TODO: Error failed to create render pass
        return nullptr;
    }

    return vk_render_pass;
}

VmaAllocator ValInstance::create_vma_allocator(VkInstance vk_instance, VkDevice vk_device, VkPhysicalDevice vk_gpu) {
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = vk_gpu;
    allocatorInfo.device = vk_device;
    allocatorInfo.instance = vk_instance;

    VmaAllocator vma_allocator = nullptr;

    if (vmaCreateAllocator(&allocatorInfo, &vma_allocator) != VK_SUCCESS) {
        return nullptr;
    }

    return vma_allocator;
}

VkSemaphore ValInstance::create_vk_semaphore(VkDevice vk_device) {
    VkSemaphoreCreateInfo semaphore_create_info{};
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkSemaphore vk_semaphore;
    if (vkCreateSemaphore(vk_device, &semaphore_create_info, nullptr, &vk_semaphore) != VK_SUCCESS) {
        return nullptr;
    }

    return vk_semaphore;
}

VkFence ValInstance::create_vk_fence(VkDevice vk_device) {
    VkFenceCreateInfo fence_create_info{};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkFence vk_fence;
    if (vkCreateFence(vk_device, &fence_create_info, nullptr, &vk_fence) != VK_SUCCESS) {
        return nullptr;
    }

    return vk_fence;
}

ValInstance *ValInstance::create_val_instance(ValInstanceCreateInfo *p_create_info) {
    if (p_create_info == nullptr) {
        return nullptr;
    }

    VkInstance vk_instance = create_vk_instance(p_create_info);
    if (vk_instance == nullptr) {
        if (last_error == ERR_NONE) {
            last_error = ERR_VK_INSTANCE_FAILURE;
        }

        return nullptr;
    }

#ifdef SDL_SUPPORT
    ValWindow* main_window = new ValWindow(p_create_info->sdl_window, vk_instance);
#endif

    // TODO: Proper multiple window support (thanks windows for making presenting harder :| )
    ChosenGPU gpu = pick_gpu(vk_instance, main_window->vk_surface, p_create_info);
    VkDevice vk_device = create_vk_device(p_create_info, gpu.vk_device, gpu.queues);

    ValInstance* instance = new ValInstance();

    instance->vk_instance = vk_instance;
    instance->vk_physical_device = gpu.vk_device;
    instance->val_queues = gpu.queues;
    instance->vk_device = vk_device;

    for (ValQueue& queue: instance->val_queues) {
        queue.create_pool(instance);
    }

    instance->vma_allocator = create_vma_allocator(vk_instance, vk_device, gpu.vk_device);

    instance->vk_image_available_semaphore = create_vk_semaphore(vk_device);
    instance->vk_render_finished_semaphore = create_vk_semaphore(vk_device);
    instance->vk_flight_fence = create_vk_fence(vk_device);

#ifdef SDL_SUPPORT
    // We ask the main window for the format and present mode it prefers
    ValWindow::PresentInfo* present_info = main_window->get_present_info(gpu.vk_device);
    instance->present_info = present_info;

    VkRenderPass vk_render_pass = create_vk_render_pass(vk_device, present_info);
    instance->vk_render_pass = vk_render_pass;

    main_window->recreate_swapchain(instance);
    instance->val_main_window = main_window;
#endif

    return instance;
}
ValQueue ValInstance::get_queue(ValQueue::QueueType type) {
    for (ValQueue& queue: val_queues) {
        if (queue.type == type) {
            return queue;
        }
    }

    return {};
}

void ValInstance::await_frame() {
    if (vk_device != nullptr && vk_flight_fence != nullptr) {
        vkWaitForFences(vk_device, 1, &vk_flight_fence, VK_TRUE, UINT64_MAX);
    }
}