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

#include "render_pass.hpp"

#include <vulkan/vulkan.h>

#include <graphics/vulkan_provider.hpp>

#include <engine.hpp>

#include <iostream>
#include <stdexcept>

using namespace Sapphire;

void Graphics::RenderPassBuilder::push_attachment(AttachmentInfo attachment_info) {
    VkAttachmentDescription attachment{};
    attachment.format = attachment_info.format;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    attachment.loadOp = attachment_info.load_clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.storeOp = attachment_info.store ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;

    attachment.stencilLoadOp = attachment_info.stencil_load_clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = attachment_info.stencil_store ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;

    attachment.initialLayout = attachment_info.initial_layout;
    attachment.finalLayout = attachment_info.final_layout;

    VkAttachmentReference attachment_ref{};
    attachment_ref.attachment = static_cast<uint32_t>(vk_attachment_descriptions.size());
    attachment_ref.layout = attachment_info.ref_layout;

    if (has_depth_stencil) {
        VkAttachmentDescription depth_attachment_description = vk_attachment_descriptions.back();
        VkAttachmentReference depth_attachment_ref = vk_attachment_refs.back();

        vk_attachment_descriptions.back() = attachment;
        vk_attachment_refs.back() = attachment_ref;

        vk_attachment_descriptions.push_back(depth_attachment_description);
        vk_attachment_refs.push_back(depth_attachment_ref);
    } else {
        vk_attachment_descriptions.push_back(attachment);
        vk_attachment_refs.push_back(attachment_ref);
    }
}

void Graphics::RenderPassBuilder::push_color_attachment(ColorAttachmentInfo attachment_info) {
    push_attachment(attachment_info);
}

void Graphics::RenderPassBuilder::push_depth_attachment(DepthStencilAttachmentInfo attachment_info) {
    if (has_depth_stencil) {
        throw std::runtime_error("Only one depth + stencil attachment is allowed!");
    }

    push_attachment(attachment_info);
    has_depth_stencil = true;
}

void Graphics::RenderPassBuilder::push_subpass(SubPassInfo subpass_info) {
    // Note: this is a heap allocated object, you better deallocate it :)
    uint32_t size = static_cast<uint32_t>(subpass_info.attachment_indices.size());

    bool has_depth = false;
    if (subpass_info.attach_depth && has_depth_stencil) {
        size += 1;
        has_depth = true;
    }

    VkAttachmentReference *references = new VkAttachmentReference[size];

    for (uint32_t r = 0; r < size; r++) {
        references[r] = vk_attachment_refs[subpass_info.attachment_indices[r]];
    }

    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = size;
    subpass.pColorAttachments = references;

    if (subpass_info.attach_depth) {
        subpass.colorAttachmentCount -= 1;
        subpass.pDepthStencilAttachment = &references[size - 1];
    }

    vk_subpasses.push_back(subpass);
}

void Graphics::RenderPassBuilder::push_subpass_dependency(DependencyInfo dependency_info) {
    VkSubpassDependency dependency {};

    dependency.srcSubpass = dependency_info.src_subpass;
    dependency.dstSubpass = dependency_info.dst_subpass;
    dependency.srcStageMask = dependency_info.src_stage_flags;
    dependency.dstStageMask = dependency_info.dst_stage_flags;
    dependency.srcAccessMask = dependency_info.dst_access_flags;
    dependency.dstAccessMask = dependency_info.dst_access_flags;
    dependency.dependencyFlags = dependency_info.dependency_flags;

    vk_subpass_dependencies.push_back(dependency);
}

VkRenderPass Graphics::RenderPassBuilder::build(Graphics::VulkanProvider *p_provider) {
    if (p_provider == nullptr) {
        throw std::runtime_error("p_provider was nullptr!");
    }

    std::vector<VkSubpassDescription> subpasses = vk_subpasses;

    if (subpasses.empty()) {
        VkSubpassDescription subpass {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = static_cast<uint32_t>(vk_attachment_refs.size());
        subpass.pColorAttachments = vk_attachment_refs.data();

        if (has_depth_stencil) {
            subpass.colorAttachmentCount -= 1;
            subpass.pDepthStencilAttachment = &vk_attachment_refs.back();
        }

        subpasses.push_back(subpass);
    }

    VkRenderPassCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    create_info.attachmentCount = static_cast<uint32_t>(vk_attachment_descriptions.size());
    create_info.pAttachments = vk_attachment_descriptions.data();
    create_info.subpassCount = static_cast<uint32_t>(subpasses.size());
    create_info.pSubpasses = subpasses.data();
    create_info.dependencyCount = static_cast<uint32_t>(vk_subpass_dependencies.size());
    create_info.pDependencies = vk_subpass_dependencies.data();

    VkRenderPass render_pass = nullptr;
    VkResult result = vkCreateRenderPass(p_provider->get_vk_device(), &create_info, nullptr, &render_pass);

    if (result != VK_SUCCESS) {
        LOG_GRAPHICS("vkCreateRenderPass failed with error code (" << result << ")");
        throw std::runtime_error("vkCreateRenderPass failed! Please check the log above for more info!");
    }

    return render_pass;
}