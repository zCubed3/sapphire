#include <iostream>

#include <SDL.h>

#ifdef WIN32
#include <engine/platforms/win32_platform.h>
#endif

#include <engine/assets/asset_loader.h>
#include <engine/assets/shader_asset.h>
#include <engine/assets/texture_asset.h>
#include <engine/assets/static_mesh_asset.h>
#include <engine/rendering/sdl_window_render_target.h>
#include <engine/rendering/shader.h>
#include <engine/scene/world.h>
#include <engine/typing/class_registry.h>
#include <engine/scene/mesh_actor.h>

#ifdef RS_OPENGL4_SUPPORT
#include <rs_opengl4/rendering/opengl4_render_server.h>
#endif

#ifdef RS_VULKAN_SUPPORT
#include <rs_vulkan/rendering/vulkan_render_server.h>
#endif

#include <imgui.h>
#include <backends/imgui_impl_sdl.h>

#include <config/config_file.h>

#include <glm.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/matrix_transform.hpp>

class BaseClass {
    REFLECT_BASE_CLASS(BaseClass)

    virtual float get_data() {
        return 0;
    }
};

class ChildClass : public BaseClass {
    REFLECT_CLASS(ChildClass, BaseClass)

    float get_data() override {
        return 1;
    }
};

class ChildChildClass : public ChildClass {
    REFLECT_CLASS(ChildChildClass, ChildClass)

    float get_data() override {
        return 2;
    }
};

int main(int argc, char **argv) {
    // Our platform we're currently running on
    const Platform *platform = nullptr;
#ifdef WIN32
    platform = Win32Platform::create_win32_platform();
#endif

    AssetLoader::register_engine_asset_loaders();

    // Load the engine config file (if it exists)
    ConfigFile engine_config;
    engine_config.read_from_path("engine.secf");

    // TODO: Not be case sensitive
    std::string user_render_server = engine_config.try_get_string("sRenderServer", "Rendering", "Vulkan");

    RenderServer *render_server = nullptr;
    if (user_render_server == "Vulkan") {
#ifdef RS_VULKAN_SUPPORT
        render_server = new VulkanRenderServer();
#else
        std::cout << "Error: Vulkan support was not compiled into the engine!" << std::endl;
        return 1;
#endif
    }

    if (user_render_server == "OpenGL4") {
#ifdef RS_OPENGL4_SUPPORT
        render_server = new OpenGL4RenderServer();
#else
        std::cout << "Error: OpenGL 4 support was not compiled into the engine!" << std::endl;
        return 1;
#endif
    }

    if (render_server == nullptr) {
        std::cout << "Error: Unrecognized render server '" << user_render_server << "'!" << std::endl;
        return 1;
    }

    render_server->register_rs_asset_loaders();

    // The render server requires a main window to get things started
    SDL_Init(SDL_INIT_EVERYTHING);

    uint32_t window_flags = render_server->get_sdl_window_flags();

    std::string window_name = "Sapphire (" + render_server->get_name() + ")";

    // TODO: Abstract window class?
    SDL_Window *main_window = SDL_CreateWindow(
            window_name.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            1280,
            720,
            window_flags | SDL_WINDOW_RESIZABLE
    );

    SDLWindowRenderTarget* rt_window = new SDLWindowRenderTarget(main_window);

    if (!render_server->initialize(main_window)) {
        std::cout << render_server->get_error() << std::endl;
        return 1;
    }

    // We need to load our model and our shader
    World *world = new World();

    MeshAsset* mesh = reinterpret_cast<MeshAsset*>(AssetLoader::load_asset("test.obj"));
    ShaderAsset *shader = reinterpret_cast<ShaderAsset*>(AssetLoader::load_asset("test.semd"));
    TextureAsset *texture = reinterpret_cast<TextureAsset*>(AssetLoader::load_asset("test.png"));

    MeshActor* actor = new MeshActor();
    actor->mesh_asset = mesh;
    actor->shader_asset = shader;

    // TODO: Temp and jank
    shader->shader->texture_asset = texture;

    world->add_actor(actor);

    AssetLoader::load_all_placeholders();

    render_server->populate_render_target_data(rt_window);

    rt_window->world = world;

    SDL_Event event{};
    bool keep_running = true;

    uint32_t last_tick = SDL_GetTicks();

    Transform model;
    Transform model2;

    bool resized = false;

    // ImGui my beloved :)
    ImGuiContext* imgui_context = ImGui::CreateContext();
    render_server->initialize_imgui();

    ImGuiStyle &style = ImGui::GetStyle();

    while (keep_running) {
        while (SDL_PollEvent(&event) != 0) {
            ImGui_ImplSDL2_ProcessEvent(&event);

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

        glm::vec3 euler {};
        euler.x = sin(world->elapsed_time) * 10;
        euler.y = cos(world->elapsed_time) * 10;

        last_tick = tick;

        world->delta_time = delta;

        actor->transform.quaternion = glm::quat(glm::radians(euler));

        world->elapsed_time += delta;

        //rt_window.clear_color = Color(abs(sin(world->elapsed_time)), 0, 0, 1);

        // We don't have a camera, so we need to move our render target
        rt_window->fov = 105;
        rt_window->transform.position = glm::vec3(0, 0, 2);

        render_server->begin_frame();
        render_server->begin_target(rt_window);
        render_server->begin_imgui();

        world->draw();

        //actor->transform.position = glm::vec3(0, sin(world->elapsed_time), 0);

        ImGui::Begin("Renderer Info");
        ImGui::Text("Rendering API: %s", render_server->get_name().c_str());
        ImGui::End();

        //ImGui::ShowDemoWindow();

        render_server->end_imgui();
        render_server->end_target(rt_window);
        render_server->end_frame();

        render_server->present(main_window);
    }

    AssetLoader::unload_all_placeholders();

    delete actor;
    delete texture;
    delete mesh;
    delete shader;

    delete rt_window;

    delete render_server;
    ImGui::DestroyContext(imgui_context);

    return 0;
}
