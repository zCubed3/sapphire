#ifndef AGE_OPENGL4_RENDER_SERVER_H
#define AGE_OPENGL4_RENDER_SERVER_H

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

public:
    void register_rs_asset_loaders() override;

    const char *get_name() override;
    const char *get_error() override;

    bool initialize(SDL_Window *p_window) override;

    bool present(SDL_Window *p_window) override;

    bool begin_render(RenderTarget *p_target) override;
    bool end_render(RenderTarget *p_target) override;

    void populate_mesh_buffer(MeshAsset *p_mesh_asset) const override;
};

#endif
