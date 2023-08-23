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

#ifndef SAPPHIRE_WINDOW_RENDER_TARGET_HPP
#define SAPPHIRE_WINDOW_RENDER_TARGET_HPP

#include <graphics/render_target.hpp>

#include <vulkan/vulkan.h>

#include "window.hpp"
#include <string>
#include <vector>

namespace Sapphire {
    class Engine;
    class Window;
}

namespace Sapphire::Graphics {
    class VulkanProvider;

    struct WindowRenderTargetData : public IProviderReleasable {
        uint32_t vk_frame_index = 0;
        VkExtent2D vk_extent;
        VkSurfaceCapabilitiesKHR vk_capabilities;
        VkSwapchainKHR vk_swapchain = nullptr;
        std::vector<VkImage> vk_images {};
        std::vector<VkImageView> vk_image_views {};
        std::vector<VkFramebuffer> vk_framebuffers {};

    protected:
        std::function<void (VulkanProvider *)> get_release_func() override;
    };

    class WindowRenderTarget : public RenderTarget {
    protected:
        VkSurfaceKHR vk_surface = nullptr;
        WindowRenderTargetData rt_data;

        VkExtent2D get_vk_extent() override;
        VkRenderPass get_vk_render_pass(VulkanProvider *p_provider) override;
        VkFramebuffer get_vk_framebuffer(VulkanProvider *p_provider) override;

        std::function<void(VulkanProvider*)> get_release_func() override;

    public:
        WindowRenderTarget() = default;
        WindowRenderTarget(VkSurfaceKHR vk_surface);

        void initialize(Engine *p_engine, Window *p_owner);

        void present(VulkanProvider* p_provider);

        void set_rt_data(WindowRenderTargetData rt_data);
        WindowRenderTargetData get_rt_data();

        VkSurfaceKHR get_vk_surface();
    };
}

#endif//SAPPHIRE_WINDOW_RENDER_TARGET_HPP
