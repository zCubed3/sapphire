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

std::function<void(Graphics::VulkanProvider*)> Graphics::WindowRenderTargetData::get_release_func() {
    return
        [
            vk_swapchain = vk_swapchain,
            vk_images = vk_images,
            vk_image_views = vk_image_views
        ]
        (VulkanProvider* p_provider) mutable -> void
        {
            if (vk_swapchain != nullptr) {
                vkDestroySwapchainKHR(p_provider->get_vk_device(), vk_swapchain, nullptr);
            }

            // Windows do not need to destroy their framebuffer images
            // TODO: Destroy depth buffers

            for (VkImageView vk_image_view: vk_image_views) {
                vkDestroyImageView(p_provider->get_vk_device(), vk_image_view, nullptr);
            }
        };
}

VkExtent2D Graphics::WindowRenderTarget::get_vk_extent() {
    return rt_data.vk_extent;
}

VkRenderPass Graphics::WindowRenderTarget::get_vk_render_pass(Sapphire::Graphics::VulkanProvider *p_provider) {
    if (p_provider == nullptr) {
        throw std::runtime_error("p_provider is nullptr!");
    }

    return p_provider->get_render_pass_window();
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

std::function<void(Graphics::VulkanProvider*)> Graphics::WindowRenderTarget::get_release_func() {
    return [=](VulkanProvider* p_provider) mutable{
        rt_data.release(p_provider);

        if (vk_command_buffer != nullptr) {
            // TODO: Safer command buffer allocation?
            p_provider->free_command_buffer(VulkanProvider::QueueType::Graphics, vk_command_buffer);
        }

        if (vk_surface != nullptr) {
            vkDestroySurfaceKHR(p_provider->get_vk_instance(), vk_surface, nullptr);
        }
    };
}

void Graphics::WindowRenderTarget::initialize(Graphics::VulkanProvider *p_provider, Window *p_owner) {
    if (p_provider == nullptr) {
        throw std::runtime_error("p_provider is nullptr!");
    }

    if (p_owner == nullptr) {
        throw std::runtime_error("p_owner is nullptr!");
    }

    // This assumes we haven't been given a surface beforehand
    // If your surface is nullptr after assigning it, something is wrong :P
    if (vk_surface == nullptr) {
       p_provider->create_vk_surface(p_owner);
    }

    p_provider->setup_window_render_target(this, p_owner);

    if (vk_command_buffer == nullptr) {
        vk_command_buffer = p_provider->allocate_command_buffer(VulkanProvider::QueueType::Graphics);
    }
}

Graphics::WindowRenderTarget::WindowRenderTarget(Engine *p_engine, Window *p_owner) {
    if (p_engine == nullptr) {
        throw std::runtime_error("p_engine was nullptr!");
    }

    initialize(p_engine->get_vk_provider(), p_owner);
}

Graphics::WindowRenderTarget::WindowRenderTarget(Engine *p_engine, Window *p_owner, VkSurfaceKHR vk_surface) {
    if (p_engine == nullptr) {
        throw std::runtime_error("p_engine was nullptr!");
    }

    this->vk_surface = vk_surface;
    initialize(p_engine->get_vk_provider(), p_owner);
}

Graphics::WindowRenderTarget::WindowRenderTarget(Graphics::VulkanProvider *p_provider, Sapphire::Window *p_owner) {
    initialize(p_provider, p_owner);
}

Graphics::WindowRenderTarget::WindowRenderTarget(Graphics::VulkanProvider *p_provider, Sapphire::Window *p_owner, VkSurfaceKHR vk_surface) {
    this->vk_surface = vk_surface;
    initialize(p_provider, p_owner);
}

void Graphics::WindowRenderTarget::recreate(Graphics::VulkanProvider *p_provider, Window *p_owner) {
    initialize(p_provider, p_owner);
}

void Graphics::WindowRenderTarget::present(Graphics::VulkanProvider *p_provider) {
    if (p_provider == nullptr) {
        throw std::runtime_error("p_provider is nullptr!");
    }

    VkSemaphore vk_semaphore_render_finished = p_provider->get_render_finished_semaphore();
    VulkanProvider::Queue queue = p_provider->get_queue(VulkanProvider::QueueType::Present);

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &vk_semaphore_render_finished;

    present_info.swapchainCount = 1;
    present_info.pSwapchains = &rt_data.vk_swapchain;
    present_info.pImageIndices = &rt_data.vk_frame_index;

    vkQueuePresentKHR(queue.vk_queue, &present_info);
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
