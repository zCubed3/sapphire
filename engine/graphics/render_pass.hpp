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

#ifndef SAPPHIRE_RENDER_PASS_HPP
#define SAPPHIRE_RENDER_PASS_HPP

#include <vulkan/vulkan.h>

#include <vector>

namespace Sapphire::Graphics {
    class VulkanProvider;

    // TODO: Multiple subpasses
    // TODO: Subpass dependencies
    struct AttachmentInfo {
        VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

        bool load_clear = true;
        bool store = true;

        bool stencil_load_clear = false;
        bool stencil_store = false;

        VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout final_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout ref_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    };


    struct ColorAttachmentInfo : AttachmentInfo {};

    struct DepthStencilAttachmentInfo : AttachmentInfo {};

    struct SubPassInfo {
        std::vector<uint32_t> attachment_indices;
        bool attach_depth = true;
    };

    struct DependencyInfo {
        uint32_t src_subpass = VK_SUBPASS_EXTERNAL;
        uint32_t dst_subpass = VK_SUBPASS_EXTERNAL;

        VkPipelineStageFlags src_stage_flags = 0;
        VkPipelineStageFlags dst_stage_flags = 0;

        VkAccessFlags src_access_flags = 0;
        VkAccessFlags dst_access_flags = 0;

        VkDependencyFlags dependency_flags = VK_DEPENDENCY_BY_REGION_BIT;
    };

    class RenderPassBuilder {
        std::vector<VkAttachmentDescription> vk_attachment_descriptions;
        std::vector<VkAttachmentReference> vk_attachment_refs;
        std::vector<VkSubpassDescription> vk_subpasses;
        std::vector<VkSubpassDependency> vk_subpass_dependencies;

        // We can only have 1 depth stencil!
        // For implementation purposes it is always last!
        bool has_depth_stencil = false;

        void push_attachment(AttachmentInfo attachment_info);

    public:
        void push_color_attachment(ColorAttachmentInfo attachment_info);
        bool push_depth_attachment(DepthStencilAttachmentInfo attachment_info);

        void push_subpass(SubPassInfo subpass_info);
        void push_subpass_dependency(DependencyInfo dependency_info);

        VkRenderPass build(VulkanProvider *p_provider);
    };
}

#endif//SAPPHIRE_RENDER_PASS_HPP
