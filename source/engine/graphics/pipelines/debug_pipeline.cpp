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

#include "debug_pipeline.hpp"

#include <stdexcept>

#include <mana/mana_instance.hpp>
#include <mana/mana_pipeline.hpp>
#include <mana/mana_render_pass.hpp>

#include <mana/internal/vulkan_render_target.hpp>
#include <mana/internal/vulkan_cmd_buffer.hpp>

#include <mana/builders/mana_render_pass_builder.hpp>

#include <vulkan/vulkan.h>

using namespace Sapphire;

void Graphics::DebugPipeline::initialize(ManaVK::ManaInstance *owner) {
    //
    // The pipeline render pass
    //
    ManaVK::Builders::ManaRenderPassBuilder render_pass_builder;
    {
        {
            ManaVK::Builders::ManaRenderPassBuilder::ColorAttachment color{};
            color.format = ManaVK::ManaColorFormat::Default;
            color.intent = ManaVK::ManaAttachmentIntent::Presentation;
            color.clearable = true;

            render_pass_builder.push_color_attachment(color);
        }
        {
            ManaVK::Builders::ManaRenderPassBuilder::DepthAttachment depth{};
            depth.format = ManaVK::ManaDepthFormat::Default;
            depth.clearable = true;

            render_pass_builder.set_depth_attachment(depth);
        }
    }

    render_pass = render_pass_builder.build(owner);
}

void Graphics::DebugPipeline::new_frame(ManaVK::ManaRenderContext &context) {
    // TODO: Implement a better API for Vulkan drawing inside of Mana?
    render_pass->begin(context);

    {
        auto vk_cmd_buffer = context.get_vulkan_rt()->get_vulkan_cmd_buffer()->get_vk_cmd_buffer();

        VkClearAttachment clear {};
        {
            VkClearColorValue color;
            color.float32[0] = 1.0F;
            color.float32[3] = 1.0F;

            clear.clearValue.color = color;
            clear.colorAttachment = 0;
            clear.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        VkClearRect clear_rect {};
        {
            clear_rect.rect.offset = {0, 0};
            clear_rect.rect.extent = context.get_vulkan_rt()->get_vk_extent();
            clear_rect.layerCount = 1;
        }

        vkCmdClearAttachments(vk_cmd_buffer, 1, &clear, 1, &clear_rect);
    }

    render_pass->end(context);
}