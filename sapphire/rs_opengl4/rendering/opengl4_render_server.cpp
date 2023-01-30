#include "opengl4_render_server.h"

#include <SDL.h>
#include <glad/glad.h>

#include <engine/assets/asset_loader.h>
#include <engine/assets/mesh_asset.h>
#include <engine/rendering/render_target.h>

#include <rs_opengl4/assets/glsl_shader_asset_loader.h>
#include <rs_opengl4/rendering/opengl4_mesh_buffer.h>
#include <rs_opengl4/rendering/opengl4_graphics_buffer.h>

#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>

void OpenGL4RenderServer::register_rs_asset_loaders() {
    AssetLoader::register_loader<GLSLShaderAssetLoader>();
}

std::string OpenGL4RenderServer::get_name() const {
    return "OpenGL 4.6";// TODO: Other versions?
}

std::string OpenGL4RenderServer::get_error() const {
    switch (error) {
        case ERR_SINGLETON_CLASH:
            return "Singleton not nullptr!";

        case ERR_WINDOW_NULLPTR:
            return "window was nullptr!";

        case ERR_CONTEXT_NULLPTR:
            return "gl_context was nullptr!";
    }

    return "";
}

uint32_t OpenGL4RenderServer::get_sdl_window_flags() const {
    return SDL_WINDOW_OPENGL;
}

// TODO: Abstract window class?
// Should the window be abstracted or should viewports altogether be abstracted?

bool OpenGL4RenderServer::initialize(SDL_Window *p_window) {
    if (singleton != nullptr) {
        error = ERR_SINGLETON_CLASH;
        return false;
    }

    if (p_window == nullptr) {
        error = ERR_WINDOW_NULLPTR;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    gl_context = SDL_GL_CreateContext(p_window);

    if (gl_context == nullptr) {
        error = ERR_CONTEXT_NULLPTR;
        return false;
    }

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        return false;
    }

    singleton = this;
    window = p_window;

    // TODO: VSync option
    SDL_GL_SetSwapInterval(1);

    // TODO: Move this to render targets
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    return true;
}

void OpenGL4RenderServer::initialize_imgui() {
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");
}

bool OpenGL4RenderServer::present(SDL_Window *p_window) {
    if (p_window == nullptr) {
        error = ERR_WINDOW_NULLPTR;
        return false;
    }

    SDL_GL_SwapWindow(p_window);

    return true;
}

bool OpenGL4RenderServer::begin_frame() {
    return true;
}

bool OpenGL4RenderServer::end_frame() {
    return true;
}

bool OpenGL4RenderServer::begin_target(RenderTarget *p_target) {
    if (p_target == nullptr) {
        error = ERR_TARGET_NULLPTR;
        return false;
    }

    // TODO: Not set this per frame?
    // TODO: If and when Vulkan is added, make this conformant to Vulkan and not OpenGL!
    Rect rect = p_target->get_rect();
    glViewport(rect.x, rect.y, rect.width, rect.height);

    if (p_target->clear_flags != 0) {
        uint32_t clear_flags = 0;

        if (p_target->clear_flags & RenderTarget::ClearFlags::CLEAR_FLAG_DEPTH) {
            clear_flags |= GL_DEPTH_BUFFER_BIT;
        }

        if (p_target->clear_flags & RenderTarget::ClearFlags::CLEAR_FLAG_COLOR) {
            clear_flags |= GL_COLOR_BUFFER_BIT;
            glClearColor(p_target->clear_color[0], p_target->clear_color[1], p_target->clear_color[2], p_target->clear_color[3]);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    p_target->begin_attach();

    // TODO: Let's find a better way to do this :P
    current_target = p_target;

    return true;
}

bool OpenGL4RenderServer::end_target(RenderTarget *p_target) {
    if (p_target == nullptr) {
        error = ERR_TARGET_NULLPTR;
        return false;
    }

    current_target = nullptr;

    return true;
}

bool OpenGL4RenderServer::begin_imgui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    return true;
}

bool OpenGL4RenderServer::end_imgui() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return true;
}

GraphicsBuffer *OpenGL4RenderServer::create_graphics_buffer(size_t size) const {
    uint32_t buffer_handle;
    glGenBuffers(1, &buffer_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer_handle);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    OpenGL4GraphicsBuffer *buffer = new OpenGL4GraphicsBuffer();
    buffer->buffer_handle = buffer_handle;

    return buffer;
}

void OpenGL4RenderServer::populate_mesh_buffer(MeshAsset *p_mesh_asset) const {
    if (p_mesh_asset != nullptr) {
        p_mesh_asset->buffer = new OpenGL4MeshBuffer(p_mesh_asset);
    }
}

void OpenGL4RenderServer::populate_render_target_data(RenderTarget *p_render_target) const {
    if (p_render_target != nullptr) {
        RenderServer::populate_render_target_data(p_render_target);
    }
}