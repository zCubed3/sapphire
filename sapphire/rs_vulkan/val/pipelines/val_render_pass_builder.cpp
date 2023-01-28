#include "val_render_pass_builder.h"

#include <rs_vulkan/val/val_instance.h>

void ValRenderPassBuilder::push_attachment(ValRenderPassAttachmentInfo *p_attachment_info) {
    if (p_attachment_info == nullptr) {
        return;
    }

    VkAttachmentDescription attachment{};
    attachment.format = p_attachment_info->format;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    attachment.loadOp = p_attachment_info->load_clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.storeOp = p_attachment_info->store ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;

    attachment.stencilLoadOp = p_attachment_info->stencil_load_clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = p_attachment_info->stencil_store ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;

    attachment.initialLayout = p_attachment_info->initial_layout;
    attachment.finalLayout = p_attachment_info->final_layout;

    VkAttachmentReference attachment_ref{};
    attachment_ref.attachment = static_cast<uint32_t>(vk_attachment_descriptions.size());
    attachment_ref.layout = p_attachment_info->ref_layout;

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

void ValRenderPassBuilder::push_color_attachment(ValRenderPassColorAttachmentInfo *p_attachment_info) {
    push_attachment(p_attachment_info);
}

bool ValRenderPassBuilder::push_depth_attachment(ValRenderPassDepthStencilAttachmentInfo *p_attachment_info) {
    if (!has_depth_stencil) {
        push_attachment(p_attachment_info);
        has_depth_stencil = true;
        return true;
    }

    return false;
}

ValRenderPass *ValRenderPassBuilder::build(ValInstance *p_val_instance) {
    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = static_cast<uint32_t>(vk_attachment_refs.size());
    subpass.pColorAttachments = vk_attachment_refs.data();

    if (has_depth_stencil) {
        subpass.colorAttachmentCount -= 1;
        subpass.pDepthStencilAttachment = &vk_attachment_refs.back();
    }

    VkRenderPassCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    create_info.attachmentCount = static_cast<uint32_t>(vk_attachment_descriptions.size());
    create_info.pAttachments = vk_attachment_descriptions.data();
    create_info.subpassCount = 1;
    create_info.pSubpasses = &subpass;

    VkRenderPass vk_render_pass;
    if (vkCreateRenderPass(p_val_instance->vk_device, &create_info, nullptr, &vk_render_pass) != VK_SUCCESS) {
        // TODO: Error failed to create render pass
        return nullptr;
    }

    ValRenderPass* render_pass = new ValRenderPass();
    render_pass->vk_render_pass = vk_render_pass;

    return render_pass;
}
