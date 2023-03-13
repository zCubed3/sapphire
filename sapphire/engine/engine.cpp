#include "engine.hpp"

#include <engine/rendering/render_server.hpp>

#include <SDL.h>

using namespace Sapphire;
using namespace Sapphire::Rendering;

Engine::Engine() {
    render_server = std::make_unique<RenderServer>();
}

void Engine::engine_loop() {
    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            render_server->handle_event(&event);
        }

        render_server->begin_frame();

        render_server->end_frame();
    }
}