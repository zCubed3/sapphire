#ifndef SAPPHIRE_OPENGL4_RENDER_SERVER_H
#define SAPPHIRE_OPENGL4_RENDER_SERVER_H

#include <engine/rendering/render_server.h>

class OpenGL4RenderServer : public RenderServer {
protected:
    enum Error {
        ERR_NONE,
        ERR_SINGLETON_CLASH,
        ERR_WINDOW_NULLPTR,
        ERR_CONTEXT_NULLPTR,
        ERR_TARGET_NULLPTR
    };

    int error = Error::ERR_NONE;
    void *gl_context = nullptr;
    SDL_Window *window = nullptr;

public:
    void register_rs_asset_loaders() override;

    std::string get_name() const override;
    std::string get_error() const override;
    uint32_t get_sdl_window_flags() const override;

    bool initialize(SDL_Window *p_window) override;
    void initialize_imgui() override;

    bool present(SDL_Window *p_window) override;

    bool begin_frame() override;
    bool end_frame() override;

    bool begin_target(RenderTarget *p_target) override;
    bool end_target(RenderTarget *p_target) override;

    bool begin_imgui() override;
    bool end_imgui() override;

    GraphicsBuffer *create_graphics_buffer(size_t size) const override;
    Shader *create_shader() const override;
    void populate_mesh_buffer(MeshAsset *p_mesh_asset) const override;
    void populate_render_target_data(RenderTarget *p_render_target) const override;
};

#endif
