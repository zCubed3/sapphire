#include <iostream>

#include <SDL.h>

#include <engine/assets/asset_loader.h>
#include <engine/assets/shader_asset.h>
#include <engine/assets/static_mesh_asset.h>
#include <engine/rendering/rt_sdl_window.h>
#include <engine/scene/world.h>

#include <rs_opengl4/rendering/opengl4_render_server.h>
#include <rs_vulkan/rendering/vulkan_render_server.h>

#include <glm.hpp>

#include <gtx/quaternion.hpp>
#include <gtc/matrix_transform.hpp>

int main(int argc, char **argv) {
    AssetLoader::register_engine_asset_loaders();

    SDL_Init(SDL_INIT_EVERYTHING);

    // The render server requires a main window to get things started
    // TODO: Splash screen?
    //RenderServer *render_server = new OpenGL4RenderServer();
    RenderServer *render_server = new VulkanRenderServer();
    render_server->register_rs_asset_loaders();

    uint32_t window_flags = render_server->get_sdl_window_flags();

    // TODO: Abstract window class?
    SDL_Window *main_window = SDL_CreateWindow(
            "Sapphire",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            1280,
            720,
            window_flags | SDL_WINDOW_RESIZABLE
    );

    SDLWindowRenderTarget rt_window(main_window);

    // Each render target has clear operations associated with it
    rt_window.clear_flags = RenderTarget::CLEAR_DEPTH | RenderTarget::CLEAR_COLOR;

    if (!render_server->initialize(main_window)) {
        std::cout << render_server->get_error() << std::endl;
        return 1;
    }

    // We need to load our model and our shader
    MeshAsset *mesh = static_cast<MeshAsset *>(AssetLoader::load_asset("test.obj"));

    ShaderAsset *shader = static_cast<ShaderAsset *>(AssetLoader::load_asset("test.spv"));
    //ShaderAsset *shader = static_cast<ShaderAsset *>(AssetLoader::load_asset("test.glsl"));

    World *world = new World();

    rt_window.world = world;

    SDL_Event event{};
    bool keep_running = true;

    uint32_t last_tick = SDL_GetTicks();

    glm::mat4 model = glm::identity<glm::mat4>();

    bool resized = false;

    while (keep_running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                keep_running = false;// TEMPORARY!
            }

            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    resized = true;
                }
            }
        }

        if (resized) {
            render_server->on_window_resized();
            resized = false;
        }

        // TODO: Update the world properly
        uint32_t tick = SDL_GetTicks();
        float delta = (tick - last_tick) / 1000.0F;


        last_tick = tick;

        world->elapsed_time += delta;
        world->delta_time = delta;

        glm::vec3 euler = glm::vec3(0, world->elapsed_time * 90, 0);

        //rt_window.clear_color = Color(abs(sin(world->elapsed_time)), 0, 0, 1);

        model = glm::identity<glm::mat4>();
        model *= glm::toMat4(glm::quat(glm::radians(euler)));

        render_server->begin_frame();

        render_server->begin_target(&rt_window);

        // TODO: MeshRenderer
        mesh->shader = shader;
        mesh->render(model);

        render_server->end_target(&rt_window);

        render_server->end_frame();

        render_server->present(main_window);
    }

    delete render_server;

    return 0;
}
