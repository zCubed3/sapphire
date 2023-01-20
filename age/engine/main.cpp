#include <iostream>
#include <chrono>

#include <age_std/String.h>
#include <age_std/Vector.h>

#include <SDL.h>

#include <rendering/rt_sdl_window.h>
#include <rs_opengl4/rs_opengl4.h>

using namespace AGE;

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_EVERYTHING);

    // The render server requires a main window to get things started
    // TODO: AGE splash screen?
    RenderServer* server = new OpenGL4RenderServer();

    // TODO: Abstract window class!
    SDL_Window* main_window = SDL_CreateWindow(
            "AGE Window",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            800,
            600,
            SDL_WINDOW_OPENGL
    );

    SDLWindowRenderTarget rt_window(main_window);

    // Each render target has clear operations associated with it
    rt_window.clear_flags = RenderTarget::CLEAR_DEPTH | RenderTarget::CLEAR_COLOR;

    server->initialize(main_window);

    SDL_Event event {};
    bool keep_running = true;

    while (keep_running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT)
                keep_running = false; // TEMPORARY!
        }

        server->present(main_window);
    }

    return 0;
}
