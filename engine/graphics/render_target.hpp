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

#ifndef SAPPHIRE_RENDER_TARGET_HPP
#define SAPPHIRE_RENDER_TARGET_HPP

#include <vulkan/vulkan.h>

namespace Sapphire::Graphics {
    class VulkanProvider;

    // A target / surface that can be drawn to
    // This encapsulates common behavior between render textures and OS windows!
    class RenderTarget {
    protected:
        VkCommandBuffer vk_command_buffer = nullptr;

        VkClearColorValue clear_color = {0, 0, 0, 1};
        VkClearDepthStencilValue clear_depth_stencil = {1.0F, 0};

    public:
        virtual VkFramebuffer get_vk_framebuffer(VulkanProvider *p_provider) = 0;

        virtual VkCommandBuffer get_vk_command_buffer();
    };
}

#endif//SAPPHIRE_RENDER_TARGET_HPP
