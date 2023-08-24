/*
MIT License

Copyright (c) 2023 zCubed (Liam R.)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef SAPPHIRE_VULKAN_PROVIDER_HPP
#define SAPPHIRE_VULKAN_PROVIDER_HPP

#include <vector>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <functional>

namespace Sapphire {
    class Engine;
    class Window;
}

namespace Sapphire::Graphics {
    class WindowRenderTarget;

    // A wrapper around Vulkan instance creation / management
    class VulkanProvider {
    public:
        using ReleaseFunction = std::function<void(VulkanProvider*)>;

        enum class QueueType {
            Unknown,
            Graphics,
            Transfer,
            Present
        };

        struct Queue {
            uint32_t family = -1;
            QueueType type = QueueType::Unknown;
            VkQueue vk_queue = nullptr;
            VkCommandPool vk_pool = nullptr;
        };

        struct PresentInfo {
            VkFormat vk_color_format;
            VkColorSpaceKHR vk_colorspace;
            VkFormat vk_depth_format;
            VkPresentModeKHR vk_present_mode;
        };

    protected:
        VkPhysicalDeviceFeatures vk_gpu_features;
        VkPhysicalDevice vk_gpu = nullptr;
        VkDevice vk_device = nullptr;
        VkInstance vk_instance = nullptr;

        VkSemaphore vk_image_available_semaphore = nullptr;
        VkSemaphore vk_render_finished_semaphore = nullptr;
        VkFence vk_render_fence = nullptr;
        VkDescriptorPool vk_descriptor_pool = nullptr;

        VmaAllocator vma_allocator = nullptr;

        std::vector<VkSurfaceFormatKHR> vk_supported_surface_formats;
        std::vector<VkPresentModeKHR> vk_supported_present_modes;

        PresentInfo present_info;

        Queue queue_graphics;
        Queue queue_present;
        Queue queue_transfer;

        // TODO: User defined vertex data?
        VkVertexInputBindingDescription vk_vtx_binding;
        std::vector<VkVertexInputAttributeDescription> vk_vtx_attributes;

        bool defer_release = false;
        std::vector<ReleaseFunction> deferred_releases;

        VkRenderPass vk_render_pass_window = nullptr;
        // TODO: Image render pass

        bool validate_instance_extensions(const std::vector<const char *> &extensions, Engine *p_engine);
        bool validate_instance_layers(const std::vector<const char *> &layers, Engine *p_engine);
        bool validate_device_extensions(const std::vector<const char *> &extensions, Engine *p_engine);

        void cache_surface_info(VkSurfaceKHR vk_surface);
        VkFormat find_supported_surface_format(const std::vector<VkFormat> &vk_formats);
        VkFormat find_supported_format(const std::vector<VkFormat> &vk_formats, VkImageTiling vk_tiling, VkFormatFeatureFlags vk_feature_flags);
        VkPresentModeKHR find_supported_present_mode(const std::vector<VkPresentModeKHR> &vk_present_modes);
        void determine_present_info();

        void create_instance(Engine *p_engine);
        void find_gpu(Engine *p_engine, VkSurfaceKHR vk_surface);
        void create_device(Engine *p_engine);
        void create_vma_allocator(Engine *p_engine);
        void create_vk_descriptor_pool();
        void create_render_passes();
        void create_vk_vtx_info();

        VkSemaphore create_vk_semaphore();
        VkFence create_vk_fence();

    public:
        VkSurfaceKHR create_vk_surface(Window *p_window);
        void setup_window_render_target(WindowRenderTarget *p_target, Window *p_window);

        VkCommandBuffer allocate_command_buffer(QueueType queue_type);
        void free_command_buffer(QueueType queue_type, VkCommandBuffer vk_command_buffer);

        void reset_render_fence();

        void initialize(Engine *p_engine);

        VkInstance get_vk_instance();
        VkDevice get_vk_device();
        VkSemaphore get_image_available_semaphore();
        VkSemaphore get_render_finished_semaphore();
        VkFence get_render_fence();
        VkRenderPass get_render_pass_window();
        Queue get_queue(QueueType type);
        VkVertexInputBindingDescription get_vk_vtx_binding();
        std::vector<VkVertexInputAttributeDescription> get_vk_vtx_attributes();

        // Signals to the provider and renderer objects that we're now rendering
        void begin_frame();
        void end_frame();

        void await_frame();

        [[nodiscard]]
        bool get_defer_release() const;
        void enqueue_release(const ReleaseFunction& function);
    };
}

#endif//SAPPHIRE_VULKAN_PROVIDER_HPP
