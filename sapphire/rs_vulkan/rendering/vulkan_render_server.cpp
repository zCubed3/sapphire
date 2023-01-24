#include "vulkan_render_server.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <engine/assets/mesh_asset.h>
#include <engine/rendering/render_target.h>

#include <rs_vulkan/assets/vulkan_shader_asset.h>
#include <rs_vulkan/assets/vulkan_shader_asset_loader.h>

#include <rs_vulkan/rendering/vulkan_mesh_buffer.h>

#include <rs_vulkan/val/val_instance.h>

#define RS_VULKAN_DEBUG


// TODO: Wait for rendering to finish
VulkanRenderServer::~VulkanRenderServer() {
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

    // Vulkan bootstrapping is so fun!

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

    uint32_t enumeration_count = 0;

    singleton = this;

    return true;
}

bool VulkanRenderServer::present(SDL_Window *p_window) {
    val_instance->val_main_window->present_queue(val_instance);
    return true;
}

void VulkanRenderServer::on_window_resized() {
    // TODO: Main window recreate swapchain
    val_instance->val_main_window->recreate_swapchain(val_instance);
}

bool VulkanRenderServer::begin_frame() {
    val_instance->await_frame();
    vkResetFences(val_instance->vk_device, 1, &val_instance->vk_flight_fence);

    return true;
}

bool VulkanRenderServer::end_frame() {
    return true;
}

bool VulkanRenderServer::begin_target(RenderTarget *p_target) {
    // TODO: Actually use render targets and not ValWindow instances
    val_instance->val_main_window->begin_rendering(val_instance);

    return true;
}

bool VulkanRenderServer::end_target(RenderTarget *p_target) {
    val_instance->val_main_window->end_rendering(val_instance);

    return true;
}

void VulkanRenderServer::populate_mesh_buffer(MeshAsset *p_mesh_asset) const {
    if (p_mesh_asset != nullptr) {
        p_mesh_asset->buffer = new VulkanMeshBuffer(p_mesh_asset);
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