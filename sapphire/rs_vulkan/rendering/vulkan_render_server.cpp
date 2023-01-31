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
#include <engine/rendering/sdl_window_render_target.h>
#include <engine/rendering/buffers/view_buffer.h>
#include <engine/scene/world.h>

#include <rs_vulkan/rendering/vulkan_mesh_buffer.h>
#include <rs_vulkan/rendering/vulkan_render_target_data.h>
#include <rs_vulkan/rendering/vulkan_graphics_buffer.h>
#include <rs_vulkan/rendering/vulkan_shader.h>
#include <rs_vulkan/rendering/vulkan_texture.h>
#include <rs_vulkan/rendering/vulkan_material.h>

#include <rs_vulkan/val/pipelines/val_pipeline.h>

#if defined(IMGUI_SUPPORT)
#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_vulkan.h>
#endif

#define RS_VULKAN_DEBUG

// TODO: Wait for rendering to finish
VulkanRenderServer::~VulkanRenderServer() {
#if defined(IMGUI_SUPPORT)
    if (imgui_initalized) {
        ImGui_ImplVulkan_Shutdown();
    }
#endif

    val_window_render_pass->release(val_instance);
    delete val_window_render_pass;

    //VulkanMeshBuffer::transform_ubo->release(val_instance);
    //delete VulkanMeshBuffer::transform_ubo;

    val_descriptor_info->release(val_instance);
    delete val_descriptor_info;

    delete val_instance;
}

void VulkanRenderServer::register_rs_asset_loaders() {

}

const char* VulkanRenderServer::get_name() const {
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

glm::vec3 VulkanRenderServer::get_coordinate_correction() const {
    return {1, -1, 1};
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

    // TODO: Optional SDL?
    // TODO: Allow switching between sRGB and Linear
    ValInstancePresentPreferences present_preferences {};
    present_preferences.use_vsync = true;

    val_create_info.p_present_preferences = &present_preferences;

    val_create_info.p_sdl_window = p_window;
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

    // Val creates a render target for our main window
    // The surface is created but that's it, we need to initialize the rest
    ValRenderPassBuilder render_pass_builder {};

    ValRenderPassColorAttachmentInfo present_color_info {};
    present_color_info.format = val_instance->present_info->vk_color_format;
    present_color_info.final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    present_color_info.ref_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    ValRenderPassDepthStencilAttachmentInfo depth_stencil_color_info {};
    depth_stencil_color_info.format = val_instance->present_info->vk_depth_format;
    depth_stencil_color_info.final_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depth_stencil_color_info.ref_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    render_pass_builder.push_color_attachment(&present_color_info);
    render_pass_builder.push_depth_attachment(&depth_stencil_color_info);

    val_window_render_pass = render_pass_builder.build(val_instance);

    val_instance->val_main_window->create_swapchain(val_window_render_pass, val_instance);

    // TODO: User defined layouts
    ValDescriptorSetBuilder val_set_builder;

    val_set_builder.push_binding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

    val_descriptor_info = val_set_builder.build(val_instance)[0];

    // TODO: Allow the user to create their own vertex types
    val_default_vertex_input = {};
    val_default_vertex_input.push_attribute(ValVertexInputBuilder::ATTRIBUTE_DATA_TYPE_VEC3);
    val_default_vertex_input.push_attribute(ValVertexInputBuilder::ATTRIBUTE_DATA_TYPE_VEC3);
    val_default_vertex_input.push_attribute(ValVertexInputBuilder::ATTRIBUTE_DATA_TYPE_VEC2);

    singleton = this;

    //
    // Placeholders
    //
    VulkanShader::create_error_shader();

    return true;
}

bool VulkanRenderServer::present(SDL_Window *p_window) {
    val_instance->val_main_window->present_queue(val_instance);
    return true;
}

void VulkanRenderServer::on_window_resized() {
    val_instance->val_main_window->create_swapchain(val_window_render_pass, val_instance);
}

bool VulkanRenderServer::begin_frame() {
    val_instance->await_frame();
    vkResetFences(val_instance->vk_device, 1, &val_instance->vk_render_fence);

    val_instance->block_await = true;

    return true;
}

bool VulkanRenderServer::end_frame() {
    val_instance->block_await = false;
    return true;
}

bool VulkanRenderServer::begin_target(RenderTarget *p_target) {
    p_target->begin_attach();
    current_target = p_target;

    // Our camera and world data is already updated by this moment
    // We just have to update the binding
    VulkanGraphicsBuffer* view_buffer = reinterpret_cast<VulkanGraphicsBuffer*>(p_target->view_buffer->buffer);
    //VulkanGraphicsBuffer* world_buffer = reinterpret_cast<VulkanGraphicsBuffer*>(p_target->view_buffer->buffer);

    ValDescriptorSetWriteInfo view_write_info{};
    view_write_info.val_buffer = view_buffer->val_buffer;

    val_descriptor_info->write_binding(&view_write_info);
    val_descriptor_info->update_set(val_instance);

    VulkanRenderTargetData *target_data = static_cast<VulkanRenderTargetData*>(p_target->data);

    if (target_data != nullptr) {
        if (target_data->val_render_target != nullptr) {
            val_active_render_target = target_data->val_render_target;

            val_active_render_target->clear_color = { p_target->clear_color[0], p_target->clear_color[1], p_target->clear_color[2], p_target->clear_color[3] };
            val_active_render_target->begin_render(val_window_render_pass, val_instance);

            // TODO: Make a dummy shader?
            vkCmdBindDescriptorSets(
                    val_active_render_target->vk_command_buffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    VulkanShader::error_shader->val_pipeline->vk_pipeline_layout,
                    0,
                    1,
                    &val_descriptor_info->val_descriptor_set->vk_descriptor_set,
                    0,
                    nullptr);
        }
    }

    return true;
}

bool VulkanRenderServer::end_target(RenderTarget *p_target) {
    VulkanRenderTargetData *target_data = static_cast<VulkanRenderTargetData*>(p_target->data);

    if (target_data != nullptr) {
        if (target_data->val_render_target != nullptr) {
            target_data->val_render_target->end_render(val_instance);

            // TODO: Not immediately submit
            if (target_data->val_render_target != val_instance->val_main_window) {
                // TODO: Not always assume this is a window
                val_instance->await_frame();

                ValWindowRenderTarget *window_rt = reinterpret_cast<ValWindowRenderTarget *>(target_data->val_render_target);
                window_rt->present_queue(val_instance);
            }
        }
    }

    current_target = nullptr;
    val_active_render_target = nullptr;

    return true;
}

GraphicsBuffer *VulkanRenderServer::create_graphics_buffer(size_t size) const {
    return new VulkanGraphicsBuffer(size);
}

Shader *VulkanRenderServer::create_shader() const {
    return new VulkanShader();
}

Texture *VulkanRenderServer::create_texture() const {
    return new VulkanTexture();
}

Material *VulkanRenderServer::create_material() const {
    return new VulkanMaterial();
}

void VulkanRenderServer::populate_mesh_buffer(MeshAsset *p_mesh_asset) const {
    if (p_mesh_asset != nullptr) {
        p_mesh_asset->buffer = new VulkanMeshBuffer(p_mesh_asset);
    }
}

void VulkanRenderServer::populate_render_target_data(RenderTarget *p_render_target) const {
    if (p_render_target != nullptr) {
        RenderServer::populate_render_target_data(p_render_target);

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
            create_info.val_render_pass = val_window_render_pass;

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

#if defined(IMGUI_SUPPORT)
void VulkanRenderServer::initialize_imgui() {
    ImGui_ImplSDL2_InitForVulkan(window);

    VkDescriptorPoolSize pool_sizes[] =
            {
                    {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                    {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                    {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                    {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                    {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                    {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
            };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = 11;
    pool_info.pPoolSizes = pool_sizes;

    VkDescriptorPool vk_imgui_descriptor_pool;
    vkCreateDescriptorPool(val_instance->vk_device, &pool_info, nullptr, &vk_imgui_descriptor_pool);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = val_instance->vk_instance;
    init_info.PhysicalDevice = val_instance->vk_physical_device;
    init_info.Device = val_instance->vk_device;
    init_info.Queue = val_instance->get_queue(ValQueue::QUEUE_TYPE_GRAPHICS).vk_queue;
    init_info.DescriptorPool = vk_imgui_descriptor_pool;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, val_window_render_pass->vk_render_pass);

    VkCommandBuffer vk_upload_buffer = begin_upload(false);

    ImGui_ImplVulkan_CreateFontsTexture(vk_upload_buffer);

    end_upload(vk_upload_buffer, false);

    ImGui_ImplVulkan_DestroyFontUploadObjects();

    imgui_initalized = true;
}

bool VulkanRenderServer::begin_imgui() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    return true;
}

bool VulkanRenderServer::end_imgui() {
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), val_active_render_target->vk_command_buffer);

    return true;
}
#endif

VkCommandBuffer VulkanRenderServer::begin_upload(bool staging) const {
    ValQueue queue;

    if (staging) {
        queue = val_instance->get_queue(ValQueue::QUEUE_TYPE_TRANSFER);
    } else {
        queue = val_instance->get_queue(ValQueue::QUEUE_TYPE_GRAPHICS);
    }

    VkCommandBuffer vk_command_buffer = queue.allocate_buffer(val_instance);

    VkCommandBufferBeginInfo buffer_begin_info{};
    buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(vk_command_buffer, &buffer_begin_info) != VK_SUCCESS) {
        // TODO: Failed to record buffer
        vkFreeCommandBuffers(val_instance->vk_device, queue.vk_pool, 1, &vk_command_buffer);
        return nullptr;
    }

    return vk_command_buffer;
}

void VulkanRenderServer::end_upload(VkCommandBuffer buffer, bool staging) const {
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &buffer;

    vkEndCommandBuffer(buffer);

    ValQueue queue;
    if (staging) {
        queue = val_instance->get_queue(ValQueue::QUEUE_TYPE_TRANSFER);
    } else {
        queue = val_instance->get_queue(ValQueue::QUEUE_TYPE_GRAPHICS);
    }

    // TODO: Multiple uploads at once?
    vkQueueSubmit(queue.vk_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue.vk_queue);

    vkFreeCommandBuffers(val_instance->vk_device, queue.vk_pool, 1, &buffer);
}