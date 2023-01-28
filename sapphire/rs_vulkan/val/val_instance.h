#ifndef SAPPHIRE_VULKAN_INSTANCE_H
#define SAPPHIRE_VULKAN_INSTANCE_H

#include <string>
#include <vector>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <rs_vulkan/val/data/val_buffer.h>
#include <rs_vulkan/val/data/val_stagingbuffer.h>
#include <rs_vulkan/val/render_targets/val_window_render_target.h>
#include <rs_vulkan/val/val_extension.h>
#include <rs_vulkan/val/val_layer.h>
#include <rs_vulkan/val/val_queue.h>

// TODO: Frames in flight?

struct ValInstancePresentPreferences {
    bool use_sRGB = false;
    bool use_32bit_depth = false;
    bool use_vsync = true;
};

struct ValPresentInfo {
    VkFormat vk_color_format;
    VkColorSpaceKHR vk_colorspace;
    VkFormat vk_depth_format;
    VkPresentModeKHR vk_present_mode;
};

struct ValInstanceCreateInfo {
    enum VulkanAPIVersion {
        API_VERSION_1_0,
        API_VERSION_1_1,
        API_VERSION_1_2,
        API_VERSION_1_3
    };

    enum VerbosityFlags {
        VERBOSITY_FLAG_LIST_GPUS = 1,
        VERBOSITY_FLAG_LIST_INSTANCE_EXTENSIONS = 2,
        VERBOSITY_FLAG_LIST_DEVICE_EXTENSIONS = 4,
        VERBOSITY_FLAG_LIST_LAYERS = 8,
    };

    std::vector<ValExtension> instance_extensions;
    std::vector<ValExtension> device_extensions;
    std::vector<ValLayer> validation_layers;

    std::vector<ValQueue::QueueType> requested_queues;

    const char* engine_name;
    const char* application_name;
    VulkanAPIVersion version = VulkanAPIVersion::API_VERSION_1_0;

    uint32_t application_major_version = 1;
    uint32_t application_minor_version = 0;
    uint32_t application_patch_version = 0;

    uint32_t engine_major_version = 1;
    uint32_t engine_minor_version = 0;
    uint32_t engine_patch_version = 0;

    uint32_t verbosity_flags = 0;

    std::vector<VkFormat> vk_color_formats;
    std::vector<VkFormat> vk_depth_formats;
    std::vector<VkPresentModeKHR> vk_present_modes;

#ifdef SDL_SUPPORT
    // TODO: Multiple window support?
    SDL_Window* p_sdl_window = nullptr;
    ValInstancePresentPreferences *p_present_preferences = nullptr;
#endif
};

class ValInstance {
protected:
    struct ChosenGPU {
        VkPhysicalDevice vk_device;
        std::vector<ValQueue> queues;
    };

    enum CreationError {
        ERR_NONE,
        ERR_CREATE_INFO_NULLPTR,
        ERR_VK_INSTANCE_FAILURE,
        ERR_VK_DEVICE_FAILURE,
        ERR_VK_GPU_MISSING,
        ERR_VK_MISSING_INSTANCE_EXTENSION
    };

    static CreationError last_error;

    ValInstance() = default;

    static std::vector<ValExtension> validate_instance_extensions(ValInstanceCreateInfo* p_create_info);
    static std::vector<ValExtension> validate_device_extensions(VkPhysicalDevice vk_gpu, ValInstanceCreateInfo* p_create_info);
    static std::vector<ValLayer> validate_layers(ValInstanceCreateInfo* p_create_info);

    static VkInstance create_vk_instance(ValInstanceCreateInfo* p_create_info);
    static ChosenGPU pick_gpu(VkInstance vk_instance, VkSurfaceKHR vk_surface, ValInstanceCreateInfo* p_create_info);
    static VkDevice create_vk_device(VkPhysicalDevice vk_gpu, std::vector<ValQueue>& val_queues, ValInstanceCreateInfo* p_create_info);
    static VmaAllocator create_vma_allocator(VkInstance vk_instance, VkDevice vk_device, VkPhysicalDevice vk_gpu);
    static VkDescriptorPool create_vk_descriptor_pool(VkDevice vk_device);

    static VkSemaphore create_vk_semaphore(VkDevice vk_device);
    static VkFence create_vk_fence(VkDevice vk_device);

    // Helper for caching present data
    bool cache_surface_info(VkInstance vk_instance, VkSurfaceKHR vk_surface, VkPhysicalDevice vk_gpu);
    VkFormat find_supported_surface_format(const std::vector<VkFormat>& vk_formats);
    VkFormat find_supported_format(const std::vector<VkFormat>& vk_formats, VkImageTiling vk_tiling, VkFormatFeatureFlags vk_feature_flags);
    VkPresentModeKHR find_supported_present_mode(const std::vector<VkPresentModeKHR>& vk_present_modes);

    bool determine_present_info(ValInstancePresentPreferences *p_present_preferences);

public:
    static const char* get_error();
    static ValInstance* create_val_instance(ValInstanceCreateInfo* p_create_info);

    ValQueue get_queue(ValQueue::QueueType type);

    // Waits until the next frame is done rendering
    void await_frame();

    ~ValInstance();

    VkInstance vk_instance = nullptr;
    VkDevice vk_device = nullptr;
    VkPhysicalDevice vk_physical_device = nullptr;
    ValPresentInfo* present_info = nullptr;

    VmaAllocator vma_allocator;

    // TODO: Abstract sync objects?
    VkSemaphore vk_image_available_semaphore;
    VkSemaphore vk_render_finished_semaphore;
    VkFence vk_render_fence;

    // TODO: Abstract descriptors and pools
    VkDescriptorPool vk_descriptor_pool;

    std::vector<VkSurfaceFormatKHR> vk_supported_surface_formats;
    std::vector<VkPresentModeKHR> vk_supported_present_modes;

#ifdef SDL_SUPPORT
    ValWindowRenderTarget *val_main_window;
#endif

    std::vector<ValQueue> val_queues;
};

#endif
