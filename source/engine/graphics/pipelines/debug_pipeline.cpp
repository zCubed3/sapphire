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
#include <mana/builders/mana_render_pass_builder.hpp>

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

            render_pass_builder.push_color_attachment(color);
        }
        {
            ManaVK::Builders::ManaRenderPassBuilder::DepthAttachment depth{};
            depth.format = ManaVK::ManaDepthFormat::Default;

            render_pass_builder.set_depth_attachment(depth);
        }
    }

    render_pass = render_pass_builder.build(owner);
}

void Graphics::DebugPipeline::new_frame(ManaVK::ManaRenderContext &context) {
    // TODO: Implement a better API for Vulkan drawing inside of Mana?
}