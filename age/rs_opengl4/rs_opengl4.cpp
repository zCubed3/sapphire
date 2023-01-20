#include "rs_opengl4.h"

#include <glad/glad.h>
#include <SDL.h>

#include <engine/rendering/render_target.h>

namespace AGE {
    const char* OpenGL4RenderServer::get_name() {
        return "OpenGL 4.6"; // TODO: Other versions?
    }

    const char* OpenGL4RenderServer::get_error() {
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

    bool OpenGL4RenderServer::initialize(SDL_Window* p_window) {
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

        return true;
    }

    bool OpenGL4RenderServer::present(SDL_Window* p_window) {
        if (p_window == nullptr) {
            return false;
        }

        SDL_GL_SwapWindow(p_window);

        return true;
    }

    bool OpenGL4RenderServer::begin_render(RenderTarget* p_target) {
        if (p_target == nullptr) {
            return false;
        }

        if (p_target->clear_flags != 0) {
            uint32_t clear_flags = 0;

            if (p_target->clear_flags & RenderTarget::ClearFlags::CLEAR_DEPTH) {
                clear_flags |= GL_DEPTH_BUFFER_BIT;
            }

            if (p_target->clear_flags & RenderTarget::ClearFlags::CLEAR_COLOR) {
                clear_flags |= GL_COLOR_BUFFER_BIT;
                glClearColor(0, 0, 0, 1); // TODO: Clear color
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        return true;
    }

    bool OpenGL4RenderServer::end_render() {
        return true;
    }
}