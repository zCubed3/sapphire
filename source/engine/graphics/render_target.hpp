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

#include <graphics/provider_releasable.hpp>
#include <world/transform.hpp>

namespace Sapphire::Graphics {
    class VulkanProvider;

    // TODO: Move view data out of the render target?

    // A target / surface that can be drawn to
    // This encapsulates common behavior between render textures and OS windows!
    class RenderTarget : public IProviderReleasable {
    public:
        enum ClearFlags : int {
            None = 0,

            ClearColor = 1,
            ClearDepth = 2,

            All = ~0
        };

    protected:
        int clear_flags = ClearFlags::All;
        VkCommandBuffer vk_command_buffer = nullptr;

        VkClearColorValue clear_color = {0.1F, 0.1F, 0.1F, 1};
        VkClearDepthStencilValue clear_depth_stencil = {1.0F, 0};

        World::Transform transform;
        glm::mat4 projection;
        glm::mat4 world_to_camera;
        glm::mat4 camera_to_world;

        bool dirty_matrix = false;

        virtual VkExtent2D get_vk_extent() = 0;
        virtual VkRenderPass get_vk_render_pass(VulkanProvider *p_provider) = 0;
        virtual VkFramebuffer get_vk_framebuffer(VulkanProvider *p_provider) = 0;

        virtual void recalculate_matrices();

    public:
        virtual void begin_target(VulkanProvider *p_provider);
        virtual void end_target(VulkanProvider *p_provider);

        // Submits the recorded command buffer for rendering
        virtual void render(VulkanProvider *p_provider);

        [[nodiscard]]
        virtual VkCommandBuffer get_vk_command_buffer() const;

        void set_clear_flags(int clear_flags);
        int get_clear_flags() const;

        //
        // View manipulation
        //
        void set_view_position(glm::vec3 position);
        glm::vec3 get_view_position() const;
    };
}

#endif//SAPPHIRE_RENDER_TARGET_HPP
