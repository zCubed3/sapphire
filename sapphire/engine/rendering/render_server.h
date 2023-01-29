#ifndef SAPPHIRE_RENDER_SERVER_H
#define SAPPHIRE_RENDER_SERVER_H

#include <cstdint>
#include <string>

#include <glm.hpp>

typedef struct SDL_Window SDL_Window;

class MeshAsset;
class RenderTarget;
class GraphicsBuffer;

// Abstraction over various rendering APIs
class RenderServer {
protected:
    static RenderServer *singleton;

    RenderTarget *current_target = nullptr;

public:
    static const RenderServer *get_singleton();

    virtual ~RenderServer();

    virtual void register_rs_asset_loaders() = 0;

    virtual RenderTarget *get_current_target() const;
    virtual std::string get_name() const = 0;
    virtual std::string get_error() const = 0;
    virtual uint32_t get_sdl_window_flags() const = 0;

    // Returns the correction for different coordinate systems
    virtual glm::vec3 get_coordinate_correction() const;

    // TODO: Remove SDL dependency?
    virtual bool initialize(SDL_Window *p_window) = 0;
    virtual void initialize_imgui() = 0;

    virtual bool present(SDL_Window *p_window) = 0;

    virtual void on_window_resized();

    virtual bool begin_frame() = 0;
    virtual bool end_frame() = 0;

    // Whenever we begin rendering to a target, we call this function
    virtual bool begin_target(RenderTarget *p_target) = 0;

    // Called whenever rendering to a target is finished
    virtual bool end_target(RenderTarget *p_target) = 0;

    // Called to begin rendering ImGui
    virtual bool begin_imgui() = 0;
    virtual bool end_imgui() = 0;

    // Creates a graphics buffer for generic usage within the render pipeline
    virtual GraphicsBuffer *create_graphics_buffer(size_t size) const = 0;

    // Whenever a mesh is loaded various structures must be created
    virtual void populate_mesh_buffer(MeshAsset *p_mesh_asset) const = 0;

    // Whenever a render target is created various structures must be created
    virtual void populate_render_target_data(RenderTarget *p_render_target) const = 0;
};

#endif
