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

    instance_create_info.ppEnabledExtensionNames = enabled_layers.data();
    instance_create_info.enabledLayerCount = static_cast<uint32_t>(enabled_layers.size());


    if (vkCreateInstance(&instance_create_info, nullptr, &vk_instance) != VK_SUCCESS) {
        return nullptr;
    }

    return vk_instance;
}

VkPhysicalDevice ValInstance::pick_gpu(VkInstance vk_instance, VkSurfaceKHR vk_surface, ValInstanceCreateInfo *p_create_info) {
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

                switch (type) {
                    case ValQueue::QUEUE_TYPE_GRAPHICS: {
                        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                            queues.push_back(queue);
                        }

                        break;
                    }

                    case ValQueue::QUEUE_TYPE_TRANSFER: {
                        if (queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT) {
                            queues.push_back(queue);
                        }

                        break;
                    }

#if SDL_SUPPORT
                    case ValQueue::QUEUE_TYPE_PRESENT: {
                        VkBool32 surface_support = false;
                        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, queue_index, vk_surface, &surface_support);

                        if (surface_support) {
                            queues.push_back(queue);
                        }

                        break;
                    }
#endif
                }
            }

            queue_index++;
        }

        // TODO: Optional required features
        // TODO: GPU ranking / picking
        // TODO: Config option to manually decide GPU?
        // TODO: Go through ALL GPUs and don't pick GPU0 as the default always!
        break;
    }
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

#if SDL_SUPPORT
    ValWindow window;
    if (!window->create_surface(p_create_info->sdl_window, vk_instance)) {
        return nullptr;
    }
#endif

    ValInstance* instance = new ValInstance();

    instance->vk_instance = vk_instance;

#if SDL_SUPPORT
    instance->val_window = window;
#endif

    return instance;
}