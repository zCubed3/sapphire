#ifndef AGE_RENDER_SERVER_H
#define AGE_RENDER_SERVER_H

typedef struct SDL_Window SDL_Window;

namespace AGE {
    class RenderTarget;

    class RenderServer {
    protected:
        RenderServer* singleton = nullptr;

    public:
        virtual const char* get_name() = 0;
        virtual const char* get_error() = 0;

        virtual bool initialize(SDL_Window* p_window) = 0;

        virtual bool present(SDL_Window* p_window) = 0;

        // Whenever we begin rendering to a target, we call this function
        virtual bool begin_render(RenderTarget* p_target) = 0;
        virtual bool end_render(RenderTarget* p_target) = 0;
    };
}

#endif
