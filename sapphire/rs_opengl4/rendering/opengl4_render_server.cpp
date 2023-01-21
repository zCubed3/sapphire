#include "opengl4_render_server.h"

#include <SDL.h>
#include <glad/glad.h>

#include <engine/assets/asset_loader.h>
#include <engine/assets/mesh_asset.h>
#include <engine/rendering/render_target.h>

#include <rs_opengl4/assets/glsl_shader_asset_loader.h>
#include <rs_opengl4/rendering/opengl4_mesh_buffer.h>

void OpenGL4RenderServer::register_rs_asset_loaders() {
    AssetLoader::register_loader<GLSLShaderAssetLoader>();
}

const char *OpenGL4RenderServer::get_name() {
    return "OpenGL 4.6";// TODO: Other versions?
}

const char *OpenGL4RenderServer::get_error() {
    switch (error) {
        case ERR_SINGLETON_CLASH:
            return "Singleton not nullptr!";

        case ERR_WINDOW_NULLPTR:
            return "window was nullptr!";

        case ERR_CONTEXT_NULLPTR:
            return "gl_context was nullptr!";
    }

    return nullptr;
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

    // TODO: Move this to render targets
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    return true;
}

bool OpenGL4RenderServer::present(SDL_Window *p_window) {
    if (p_window == nullptr) {
        error = ERR_WINDOW_NULLPTR;
        return false;
    }

    SDL_GL_SwapWindow(p_window);

    return true;
}

bool OpenGL4RenderServer::begin_render(RenderTarget *p_target) {
    if (p_target == nullptr) {
        error = ERR_TARGET_NULLPTR;
        return false;
    }

    if (p_target->clear_flags != 0) {
        uint32_t clear_flags = 0;

        if (p_target->clear_flags & RenderTarget::ClearFlags::CLEAR_DEPTH) {
            clear_flags |= GL_DEPTH_BUFFER_BIT;
        }

        if (p_target->clear_flags & RenderTarget::ClearFlags::CLEAR_COLOR) {
            clear_flags |= GL_COLOR_BUFFER_BIT;
            glClearColor(p_target->color[0], p_target->color[1], p_target->color[2], p_target->color[3]);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    return true;
}

bool OpenGL4RenderServer::end_render(RenderTarget *p_target) {
    if (p_target == nullptr) {
        error = ERR_TARGET_NULLPTR;
        return false;
    }

    return true;
}

void OpenGL4RenderServer::populate_mesh_buffer(MeshAsset *p_mesh_asset) const {
    p_mesh_asset->buffer = new OpenGL4MeshBuffer(p_mesh_asset);
}