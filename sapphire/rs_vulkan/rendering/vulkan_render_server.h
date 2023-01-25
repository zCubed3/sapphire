#ifndef SAPPHIRE_VULKAN_RENDER_SERVER_H
#define SAPPHIRE_VULKAN_RENDER_SERVER_H

#include <engine/rendering/render_server.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <vector>

class ValInstance;
struct ValBuffer;

class VulkanRenderServer : public RenderServer {
public:
    SDL_Window* window;

    ValInstance* val_instance = nullptr;
    std::vector<ValBuffer*> val_camera_ubos;

public:
    ~VulkanRenderServer() override;

    void register_rs_asset_loaders() override;

    std::string get_name() const override;
    std::string get_error() const override;
    uint32_t get_sdl_window_flags() const override;

    bool initialize(SDL_Window *p_window) override;

    bool present(SDL_Window *p_window) override;

    void on_window_resized() override;

    bool begin_frame() override;
    bool end_frame() override;

    bool begin_target(RenderTarget *p_target) override;
    bool end_target(RenderTarget *p_target) override;

    void populate_mesh_buffer(MeshAsset *p_mesh_asset) const override;

    // TODO: Make this more abstract?
    VkCommandBuffer begin_upload() const;
    void end_upload(VkCommandBuffer buffer) const;
};

#endif
