#include "vulkan_render_server.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <engine/assets/mesh_asset.h>
#include <engine/rendering/render_target.h>
#include <engine/rendering/rt_sdl_window.h>
#include <engine/rendering/camera_data.h>

#include <rs_vulkan/assets/vulkan_shader_asset.h>
#include <rs_vulkan/assets/vulkan_shader_asset_loader.h>

#include <rs_vulkan/rendering/vulkan_mesh_buffer.h>
#include <rs_vulkan/rendering/vulkan_render_target_data.h>

#include <rs_vulkan/val/val_instance.h>

#define RS_VULKAN_DEBUG


// TODO: Wait for rendering to finish
VulkanRenderServer::~VulkanRenderServer() {
    val_camera_ubo->release(val_instance);
    delete val_camera_ubo;

    delete val_instance;
}

void VulkanRenderServer::register_rs_asset_loaders() {
    AssetLoader::register_loader<VulkanShaderAssetLoader>();
}

std::string VulkanRenderServer::get_name() const {
    // TODO: Include API version?
    return "Vulkan";
}

std::string VulkanRenderServer::get_error() const {
    // TODO: Vulkan render server errors
    return "";
}

uint32_t VulkanRenderServer::get_sdl_window_flags() const {
    return SDL_WINDOW_VULKAN;
}

// TODO: Vulkan render server errors
bool VulkanRenderServer::initialize(SDL_Window *p_window) {
    if (singleton != nullptr) {
        return false;
    }

    if (p_window == nullptr) {
        return false;
    }

    window = p_window;

    // Vulkan is so fun! :(

    ValInstanceCreateInfo val_create_info {};

    val_create_info.engine_name = "Sapphire Engine";
    val_create_info.application_name = "Sapphire Application";

    // TODO: Optional SDL
    val_create_info.sdl_window = p_window;
    val_create_info.instance_extensions = ValExtension::get_sdl_instance_extensions(p_window);
    val_create_info.device_extensions = {{VK_KHR_SWAPCHAIN_EXTENSION_NAME, 0}};

#ifdef RS_VULKAN_DEBUG
    val_create_info.validation_layers = {
        {"VK_LAYER_KHRONOS_validation", 0}
    };
#endif

    val_create_info.requested_queues = {
            ValQueue::QueueType::QUEUE_TYPE_GRAPHICS,
            ValQueue::QueueType::QUEUE_TYPE_PRESENT,
            ValQueue::QueueType::QUEUE_TYPE_TRANSFER,
    };

    val_instance = ValInstance::create_val_instance(&val_create_info);

    val_camera_ubo = new ValBuffer(
            sizeof(CameraData),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
            val_instance);

    // TODO: User defined layouts
    VkDescriptorSetLayoutBinding layout_binding{};
    layout_binding.binding = 0;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layout_binding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = 1;
    layout_info.pBindings = &layout_binding;

    vkCreateDescriptorSetLayout(val_instance->vk_device, &layout_info, nullptr, &vk_descriptor_set_layout);

    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = val_instance->vk_descriptor_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &vk_descriptor_set_layout;

    if (vkAllocateDescriptorSets(val_instance->vk_device, &alloc_info, &vk_descriptor_set) != VK_SUCCESS) {
        return false;
    }

    singleton = this;

    return true;
}

bool VulkanRenderServer::present(SDL_Window *p_window) {
    val_instance->val_main_window->present_queue(val_instance);
    return true;
}

void VulkanRenderServer::on_window_resized() {
    val_instance->val_main_window->recreate_swapchain(val_instance);
}

bool VulkanRenderServer::begin_frame() {
    val_instance->await_frame();
    vkResetFences(val_instance->vk_device, 1, &val_instance->vk_render_fence);

    return true;
}

bool VulkanRenderServer::end_frame() {
    return true;
}

bool VulkanRenderServer::begin_target(RenderTarget *p_target) {
    p_target->begin_attach();

    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = val_camera_ubo->vk_buffer;
    buffer_info.offset = 0;
    buffer_info.range = sizeof(CameraData);

    VkWriteDescriptorSet descriptor_write{};
    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = vk_descriptor_set;
    descriptor_write.dstBinding = 0;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pBufferInfo = &buffer_info;
    descriptor_write.pImageInfo = nullptr; // Optional
    descriptor_write.pTexelBufferView = nullptr; // Optional

    CameraData data = {
            p_target->projection,
            p_target->view,
            p_target->eye
    };

    val_camera_ubo->write(&data, val_instance);

    vkUpdateDescriptorSets(val_instance->vk_device, 1, &descriptor_write, 0, nullptr);

    VulkanRenderTargetData *target_data = static_cast<VulkanRenderTargetData*>(p_target->data);

    if (target_data != nullptr) {
        if (target_data->val_render_target != nullptr) {
            val_active_render_target = target_data->val_render_target;
            target_data->val_render_target->begin_render(val_instance);
        }
    }

    return true;
}

bool VulkanRenderServer::end_target(RenderTarget *p_target) {
    VulkanRenderTargetData *target_data = static_cast<VulkanRenderTargetData*>(p_target->data);

    if (target_data != nullptr) {
        if (target_data->val_render_target != nullptr) {
            target_data->val_render_target->end_render(val_instance);
        }
    }

    val_active_render_target = nullptr;

    return true;
}

void VulkanRenderServer::populate_mesh_buffer(MeshAsset *p_mesh_asset) const {
    if (p_mesh_asset != nullptr) {
        p_mesh_asset->buffer = new VulkanMeshBuffer(p_mesh_asset);
    }
}

void VulkanRenderServer::populate_render_target_data(RenderTarget *p_render_target) const {
    if (p_render_target != nullptr) {
        ValRenderTargetCreateInfo::RenderTargetType type;
        switch (p_render_target->get_type()) {
            default:
                type = ValRenderTargetCreateInfo::RENDER_TARGET_TYPE_IMAGE;
                break;

            case RenderTarget::TARGET_TYPE_WINDOW:
                type = ValRenderTargetCreateInfo::RENDER_TARGET_TYPE_WINDOW;
                break;
        }

        ValRenderTargetCreateInfo create_info {};

        create_info.initialize_swapchain = true;
        create_info.type = type;

        // TODO: Not always assume RENDER_TARGET_TYPE_WINDOW means we can get an SDL window?
        if (type == ValRenderTargetCreateInfo::RENDER_TARGET_TYPE_WINDOW) {
            SDLWindowRenderTarget *sdl_target = static_cast<SDLWindowRenderTarget*>(p_render_target);
            create_info.p_window = sdl_target->window;

            if (sdl_target->window == val_instance->val_main_window->sdl_window) {
                // The main window for bootstrapping is already populated
                p_render_target->data = new VulkanRenderTargetData(val_instance->val_main_window);
                return;
            }
        }

        ValRenderTarget* val_target = ValRenderTarget::create_render_target(&create_info, val_instance);
        p_render_target->data = new VulkanRenderTargetData(val_target);
    }
}

VkCommandBuffer VulkanRenderServer::begin_upload() const {
    ValQueue transfer_queue = val_instance->get_queue(ValQueue::QUEUE_TYPE_TRANSFER);
    VkCommandBuffer vk_command_buffer = transfer_queue.allocate_buffer(val_instance);

    VkCommandBufferBeginInfo buffer_begin_info{};
    buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(vk_command_buffer, &buffer_begin_info) != VK_SUCCESS) {
        // TODO: Failed to record buffer
        vkFreeCommandBuffers(val_instance->vk_device, transfer_queue.vk_pool, 1, &vk_command_buffer);
        return nullptr;
    }

    return vk_command_buffer;
}

void VulkanRenderServer::end_upload(VkCommandBuffer buffer) const {
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &buffer;

    vkEndCommandBuffer(buffer);

    ValQueue transfer_queue = val_instance->get_queue(ValQueue::QUEUE_TYPE_TRANSFER);

    // TODO: Multiple uploads at once?
    vkQueueSubmit(transfer_queue.vk_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(transfer_queue.vk_queue);

    vkFreeCommandBuffers(val_instance->vk_device, transfer_queue.vk_pool, 1, &buffer);
}