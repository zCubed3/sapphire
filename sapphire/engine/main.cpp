#include <iostream>

#include <SDL.h>

#include <assets/asset_loader.h>
#include <assets/shader_asset.h>
#include <assets/static_mesh_asset.h>
#include <rendering/rt_sdl_window.h>
#include <rs_opengl4/rendering/opengl4_render_server.h>
#include <scene/world.h>

#include <glm.hpp>

#include <gtx/quaternion.hpp>
#include <gtc/matrix_transform.hpp>

int main(int argc, char **argv) {
    AssetLoader::register_engine_asset_loaders();

    SDL_Init(SDL_INIT_EVERYTHING);

    // The render server requires a main window to get things started
    // TODO: Splash screen?
    RenderServer *render_server = new OpenGL4RenderServer();
    render_server->register_rs_asset_loaders();

    // TODO: Abstract window class?
    SDL_Window *main_window = SDL_CreateWindow(
            "Sapphire",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            800,
            600,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    SDLWindowRenderTarget rt_window(main_window);

    // Each render target has clear operations associated with it
    rt_window.clear_flags = RenderTarget::CLEAR_DEPTH | RenderTarget::CLEAR_COLOR;

    render_server->initialize(main_window);

    // We need to load our model and our shader
    MeshAsset *mesh = static_cast<MeshAsset *>(AssetLoader::load_asset("test.obj"));
    ShaderAsset *shader = static_cast<ShaderAsset *>(AssetLoader::load_asset("test.glsl"));

    World *world = new World();

    rt_window.world = world;

    SDL_Event event{};
    bool keep_running = true;

    uint32_t last_tick = SDL_GetTicks();

    glm::mat4 model = glm::identity<glm::mat4>();

    while (keep_running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT)
                keep_running = false;// TEMPORARY!
        }

        // TODO: Update the world properly
        uint32_t tick = SDL_GetTicks();
        float delta = (tick - last_tick) / 1000.0F;


        last_tick = tick;

        world->elapsed_time += delta;
        world->delta_time = delta;

        glm::vec3 euler = glm::vec3(0, world->elapsed_time * 90, 0);

        model = glm::identity<glm::mat4>();
        model *= glm::toMat4(glm::quat(glm::radians(euler)));

        render_server->begin_render(&rt_window);

        // TODO: MeshRenderer
        mesh->shader = shader;
        mesh->render(model);

        render_server->end_render(&rt_window);

        render_server->present(main_window);
    }

    return 0;
}
