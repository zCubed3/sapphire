#ifndef SAPPHIRE_VULKAN_RENDER_SERVER_H
#define SAPPHIRE_VULKAN_RENDER_SERVER_H

#include <engine/rendering/render_server.h>

class VulkanRenderServer : public RenderServer {
public:
    void register_rs_asset_loaders() override;

    std::string get_name() const override;
    std::string get_error() const override;
    uint32_t get_sdl_window_flags() const override;

    bool initialize(SDL_Window *p_window) override;

    bool present(SDL_Window *p_window) override;

    bool begin_render(RenderTarget *p_target) override;
    bool end_render(RenderTarget *p_target) override;

    void populate_mesh_buffer(MeshAsset *p_mesh_asset) const override;
};

#endif
