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

#include "window_render_target.hpp"

#include <engine.hpp>

#include <graphics/vulkan_provider.hpp>
#include <vulkan/vulkan.h>

#include <stdexcept>

using namespace Sapphire;

void Graphics::WindowRenderTargetData::release(Graphics::VulkanProvider *p_provider) {
    if (p_provider == nullptr) {
        throw std::runtime_error("p_provider is nullptr!");
    }

    if (vk_swapchain != nullptr) {
        vkDestroySwapchainKHR(p_provider->get_vk_device(), vk_swapchain, nullptr);
    }

    for (VkImage vk_image : vk_images) {
        vkDestroyImage(p_provider->get_vk_device(), vk_image, nullptr);
    }

    for (VkImageView vk_image_view : vk_image_views) {
        vkDestroyImageView(p_provider->get_vk_device(), vk_image_view, nullptr);
    }
}

Graphics::WindowRenderTarget::WindowRenderTarget(VkSurfaceKHR vk_surface) {
    this->vk_surface = vk_surface;
}

void Graphics::WindowRenderTarget::initialize(Engine *p_engine, Window *p_owner) {
    if (p_engine == nullptr) {
        throw std::runtime_error("p_engine is nullptr!");
    }

    if (p_owner == nullptr) {
        throw std::runtime_error("p_owner is nullptr!");
    }

    Graphics::VulkanProvider *provider = p_engine->get_vk_provider();

    // This assumes we haven't been given a surface beforehand
    // If your surface is nullptr after assigning it, something is wrong :P
    if (vk_surface == nullptr) {
        provider->create_vk_surface(p_owner);
    }

    provider->setup_window_render_target(this, p_owner);

    if (vk_command_buffer == nullptr) {
        vk_command_buffer = provider->allocate_command_buffer(VulkanProvider::QueueType::Graphics);
    }
}

void Graphics::WindowRenderTarget::release(Graphics::VulkanProvider *p_provider) {
    if (vk_command_buffer != nullptr) {
        // TODO: Safer command buffer allocation?
        p_provider->free_command_buffer(VulkanProvider::QueueType::Graphics, vk_command_buffer);
    }

    if (vk_surface != nullptr) {
        vkDestroySurfaceKHR(p_provider->get_vk_instance(), vk_surface, nullptr);
    }

    rt_data.release(p_provider);
}

void Graphics::WindowRenderTarget::set_rt_data(Sapphire::Graphics::WindowRenderTargetData rt_data) {
    this->rt_data = rt_data;
}

Graphics::WindowRenderTargetData Graphics::WindowRenderTarget::get_rt_data() {
    return rt_data;
}

VkSurfaceKHR Graphics::WindowRenderTarget::get_vk_surface() {
    return vk_surface;
}

VkFramebuffer Graphics::WindowRenderTarget::get_vk_framebuffer(Sapphire::Graphics::VulkanProvider *p_provider) {
    vkAcquireNextImageKHR(
            p_provider->get_vk_device(),
            rt_data.vk_swapchain,
            UINT64_MAX,
            p_provider->get_image_available_semaphore(),
            VK_NULL_HANDLE,
            &rt_data.vk_frame_index);

    return rt_data.vk_framebuffers[rt_data.vk_frame_index];
}
