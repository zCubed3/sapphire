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

#include "render_target.hpp"

#include <engine.hpp>
#include <graphics/vulkan_provider.hpp>

// TODO: TEMP TEMP TEMP
#include <graphics/shader.hpp>
#include <shader_gen/hello_tri.spv.vert.gen.h>
#include <shader_gen/hello_tri.spv.frag.gen.h>

#include <iostream>
#include <vector>

// TODO: TEMP TEMP TEMP
Sapphire::Graphics::Shader *shader = nullptr;

using namespace Sapphire;

void Graphics::RenderTarget::begin_target(Sapphire::Graphics::VulkanProvider *p_provider) {
    if (p_provider == nullptr) {
        throw std::runtime_error("p_provider is nullptr!");
    }

    // TODO: Not lock when other buffers are being processed?
    VulkanProvider::Queue queue = p_provider->get_queue(VulkanProvider::QueueType::Graphics);
    p_provider->await_frame();

    VkRenderPassBeginInfo render_pass_info {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = get_vk_render_pass(p_provider);
    render_pass_info.framebuffer = get_vk_framebuffer(p_provider);

    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = get_vk_extent();

    std::vector<VkClearValue> clear_values;

    if (clear_flags & ClearFlags::ClearColor) {
        VkClearValue clear_value{};
        clear_value.color = clear_color;

        clear_values.push_back(clear_value);
    }

    if (clear_flags & ClearFlags::ClearDepth) {
        VkClearValue clear_value{};
        clear_value.depthStencil = clear_depth_stencil;

        clear_values.push_back(clear_value);
    }

    render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
    render_pass_info.pClearValues = clear_values.data();

    VkCommandBufferBeginInfo buffer_begin_info{};
    buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkCommandBuffer vk_command_buffer = get_vk_command_buffer();

    VkResult result = vkBeginCommandBuffer(vk_command_buffer, &buffer_begin_info);

    if (result != VK_SUCCESS) {
        LOG_GRAPHICS("Error: vkBeginCommandBuffer failed with error code (" << result << ")");
        throw std::runtime_error("vkBeginCommandBuffer failed! Please check the log above for more info!");
    }

    vkCmdBeginRenderPass(vk_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    // TODO: TEMP TEMP TEMP!!!
    if (shader == nullptr) {
        std::vector<char> vertex_data(sizeof(HELLO_TRI_VERT_CONTENTS));
        memcpy(vertex_data.data(), HELLO_TRI_VERT_CONTENTS, sizeof(HELLO_TRI_VERT_CONTENTS));

        std::vector<char> fragment_data(sizeof(HELLO_TRI_FRAG_CONTENTS));
        memcpy(fragment_data.data(), HELLO_TRI_FRAG_CONTENTS, sizeof(HELLO_TRI_FRAG_CONTENTS));

        std::shared_ptr<ShaderModule> sm_vertex = std::make_shared<ShaderModule>(p_provider, ShaderModule::ModuleType::Vertex, vertex_data);
        std::shared_ptr<ShaderModule> sm_fragment = std::make_shared<ShaderModule>(p_provider, ShaderModule::ModuleType::Fragment, fragment_data);

        shader = new Graphics::Shader(p_provider, {}, sm_vertex, sm_fragment);
    }

    VkExtent2D extent = get_vk_extent();
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = extent.width;
    viewport.height = extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(vk_command_buffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;
    vkCmdSetScissor(vk_command_buffer, 0, 1, &scissor);

    shader->bind(vk_command_buffer);
    vkCmdDraw(vk_command_buffer, 3, 1, 0, 0);
}

void Graphics::RenderTarget::end_target(Sapphire::Graphics::VulkanProvider *p_provider) {
    if (p_provider == nullptr) {
        throw std::runtime_error("p_provider is nullptr!");
    }

    VkCommandBuffer vk_command_buffer = get_vk_command_buffer();

    vkCmdEndRenderPass(vk_command_buffer);
    VkResult result = vkEndCommandBuffer(vk_command_buffer);

    if (result != VK_SUCCESS) {
        LOG_GRAPHICS("Error: vkEndCommandBuffer failed with error code (" << result << ")");
        throw std::runtime_error("vkEndCommandBuffer failed! Please check the log above for more info!");
    }
}

void Graphics::RenderTarget::render(Sapphire::Graphics::VulkanProvider *p_provider) {
    if (p_provider == nullptr) {
        throw std::runtime_error("p_provider is nullptr!");
    }

    // Await the previous render
    // TODO: Allow async rendering?
    VulkanProvider::Queue queue = p_provider->get_queue(VulkanProvider::QueueType::Graphics);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    // TODO: Fix this for image targets
    if (1) {
        VkSemaphore vk_semaphore_available = p_provider->get_image_available_semaphore();
        VkSemaphore vk_semaphore_finished = p_provider->get_render_finished_semaphore();

        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &vk_semaphore_available;
        submit_info.pWaitDstStageMask = wait_stages;

        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &vk_semaphore_finished;
    } else {
        submit_info.waitSemaphoreCount = 0;
        submit_info.signalSemaphoreCount = 0;
    }

    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &vk_command_buffer;

    VkResult result = vkQueueSubmit(queue.vk_queue, 1, &submit_info, p_provider->get_render_fence());

    // TODO: Don't wait for the frame to complete
    vkQueueWaitIdle(queue.vk_queue);
}

VkCommandBuffer Graphics::RenderTarget::get_vk_command_buffer() {
    return vk_command_buffer;
}

void Graphics::RenderTarget::set_clear_flags(int clear_flags) {
    this->clear_flags = clear_flags;
}

int Graphics::RenderTarget::get_clear_flags() const {
    return clear_flags;
}