#ifndef AGE_RENDER_SERVER_H
#define AGE_RENDER_SERVER_H

typedef struct SDL_Window SDL_Window;

class RenderTarget;

class MeshAsset;

// Abstraction over various rendering APIs
class RenderServer {
protected:
    static RenderServer *singleton;

    RenderTarget *current_target = nullptr;

public:
    static const RenderServer *get_singleton();

    virtual void register_rs_asset_loaders() = 0;

    virtual RenderTarget *get_current_target() const;
    virtual const char *get_name() = 0;
    virtual const char *get_error() = 0;

    virtual bool initialize(SDL_Window *p_window) = 0;

    virtual bool present(SDL_Window *p_window) = 0;

    // Whenever we begin rendering to a target, we call this function
    virtual bool begin_render(RenderTarget *p_target) = 0;
    virtual bool end_render(RenderTarget *p_target) = 0;

    // Whenever a mesh is loaded various structures must be created
    virtual void populate_mesh_buffer(MeshAsset *p_mesh_asset) const = 0;
};

#endif
