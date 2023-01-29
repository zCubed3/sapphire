#ifndef SAPPHIRE_VULKAN_RENDER_SERVER_H
#define SAPPHIRE_VULKAN_RENDER_SERVER_H

#include <engine/rendering/render_server.h>

#include <rs_vulkan/val/render_targets/val_render_target.h>
#include <rs_vulkan/val/pipelines/val_render_pass_builder.h>
#include <rs_vulkan/val/pipelines/val_vertex_input_builder.h>
#include <rs_vulkan/val/pipelines/val_descriptor_set_builder.h>
#include <rs_vulkan/val/val_instance.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <vector>

class ValInstance;
class ValBuffer;

class VulkanRenderServer : public RenderServer {
protected:
    bool imgui_initalized = false;
    VkCommandBuffer vk_imgui_command_buffer = nullptr;

public:
    SDL_Window* window;

    ValInstance* val_instance = nullptr;

    ValDescriptorSetInfo *val_descriptor_info = nullptr;
    ValRenderPass* val_window_render_pass;

    ValVertexInputBuilder val_default_vertex_input;
    ValRenderTarget *val_active_render_target = nullptr;

    ~VulkanRenderServer() override;

    void register_rs_asset_loaders() override;

    std::string get_name() const override;
    std::string get_error() const override;
    uint32_t get_sdl_window_flags() const override;

    glm::vec3 get_coordinate_correction() const override;

    bool initialize(SDL_Window *p_window) override;
    void initialize_imgui() override;

    bool present(SDL_Window *p_window) override;

    void on_window_resized() override;

    bool begin_frame() override;
    bool end_frame() override;

    bool begin_target(RenderTarget *p_target) override;
    bool end_target(RenderTarget *p_target) override;

    bool begin_imgui() override;
    bool end_imgui() override;

    GraphicsBuffer *create_graphics_buffer(size_t size) const override;

    void populate_mesh_buffer(MeshAsset *p_mesh_asset) const override;
    void populate_render_target_data(RenderTarget *p_render_target) const override;

    // TODO: Make this more abstract?
    VkCommandBuffer begin_upload(bool staging = true) const;
    void end_upload(VkCommandBuffer buffer, bool staging = true) const;
};

#endif
