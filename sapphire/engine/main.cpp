#include <iostream>

#include <SDL.h>

#ifdef WIN32
#include <engine/platforms/win32_platform.h>
#endif

#include <engine/assets/asset_loader.h>
#include <engine/assets/material_asset.h>
#include <engine/assets/static_mesh_asset.h>
#include <engine/assets/texture_asset.h>
#include <engine/rendering/render_server.h>
#include <engine/rendering/texture_render_target.h>
#include <engine/rendering/sdl_window_render_target.h>
#include <engine/rendering/shader.h>
#include <engine/rendering/texture.h>
#include <engine/scene/mesh_actor.h>
#include <engine/scene/world.h>
#include <engine/typing/class_registry.h>
#include <engine/timing/timing.h>

#ifdef RS_OPENGL4_SUPPORT
#include <rs_opengl4/rendering/opengl4_render_server.h>
#endif

#ifdef RS_VULKAN_SUPPORT
#include <rs_vulkan/rendering/vulkan_render_server.h>
#endif

#define TEST_MULTI_WINDOW

#if defined(IMGUI_SUPPORT)
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <backends/imgui_impl_sdl.h>

#include <engine/editor/panels/actor_panel.h>
#include <engine/editor/panels/renderer_panel.h>
#include <engine/editor/panels/world_actor_panel.h>
#endif

#include <config/config_file.h>

#include <memory>

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/matrix_transform.hpp>

#ifdef TEST_MULTI_WINDOW
struct ChildWindow {
    SDL_Window* window;
    SDLWindowRenderTarget* rt;
};
#endif

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

    std::string window_name = "Sapphire (";
    window_name += render_server->get_name();
    window_name += ")";

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
    SDL_SetWindowData(main_window, "RT", rt_window);

    if (!render_server->initialize(main_window)) {
        std::cout << render_server->get_error() << std::endl;
        return 1;
    }

    // We need to load our model and our shader
    World *world = new World();

    MeshAsset* mesh = reinterpret_cast<MeshAsset*>(AssetLoader::load_asset("test.obj"));
    MaterialAsset *material = reinterpret_cast<MaterialAsset *>(AssetLoader::load_asset("test.semd"));

    MeshActor* actor = new MeshActor();
    actor->mesh_asset = mesh;
    actor->material_asset = material;

    // TODO: Temp and jank
    TextureRenderTarget* rt_texture = new TextureRenderTarget(256, 256);
    rt_texture->transform.position = {1, 0, 2};
    rt_texture->world = world;

    render_server->populate_render_target_data(rt_texture);

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

    Timing *timing = Timing::get_singleton();

    // ImGui my beloved :)
#if defined(IMGUI_SUPPORT)
    render_server->initialize_imgui(rt_window);

    std::vector<double> fps_stack;

    std::string test_obj_path;
    std::string test_semd_path;
    glm::vec3 test_position = {1, 0, 0};

    WorldActorPanel* world_actor_panel = new WorldActorPanel();
    world_actor_panel->target = world;

    ActorPanel* actor_panel = new ActorPanel();
    actor_panel->target = nullptr;
    actor_panel->world = world;

    RendererPanel *renderer_panel = new RendererPanel();
#endif

#ifdef TEST_MULTI_WINDOW
    std::vector<ChildWindow> child_windows {};
#endif

    // We don't have a camera, so we need to move our render target initially
    rt_window->fov = 105;
    rt_window->transform.position = glm::vec3(0, 0, 2);

    while (keep_running) {
        while (SDL_PollEvent(&event) != 0) {
#if defined(IMGUI_SUPPORT)
            if (SDL_GetWindowFlags(main_window) & SDL_WINDOW_INPUT_FOCUS) {
                ImGui::SetCurrentContext(rt_window->imgui_context);
                ImGui_ImplSDL2_ProcessEvent(&event);
            }

#if defined(TEST_MULTI_WINDOW)
            size_t index = 0;
            bool closed = false;
            for (ChildWindow& window: child_windows) {
                if (event.type == SDL_WINDOWEVENT) {
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window.window)) {
                        closed = true;
                        break;
                    }
                }

                if (SDL_GetWindowFlags(window.window) & SDL_WINDOW_INPUT_FOCUS) {
                    ImGui::SetCurrentContext(window.rt->imgui_context);
                    ImGui_ImplSDL2_ProcessEvent(&event);
                }

                index++;
            }

            if (closed) {
                delete child_windows[index].rt;
                SDL_DestroyWindow(child_windows[index].window);

                child_windows.erase(child_windows.begin() + index);
            }
#endif
#endif

            if (event.type == SDL_QUIT) {
                keep_running = false;// TEMPORARY!
            }

            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESTORED && event.window.windowID == SDL_GetWindowID(main_window)) {
#if defined(TEST_MULTI_WINDOW)
                    for (ChildWindow& window: child_windows) {
                        SDL_RestoreWindow(window.window);
                    }
#endif
                }

                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    render_server->on_window_resized(SDL_GetWindowFromID(event.window.windowID));
                }
            }
        }

        // TODO: Update the world properly
        timing->new_frame();
        float delta = static_cast<float>(timing->get_delta());

        glm::vec3 euler {};
        euler.x = sin(world->elapsed_time) * 10;
        euler.y = cos(world->elapsed_time) * 10;

        world->delta_time = delta;

        //actor->transform.quaternion = glm::quat(glm::radians(euler));

        world->elapsed_time += delta;

        //rt_window.clear_color = Color(abs(sin(world->elapsed_time)), 0, 0, 1);

        render_server->begin_frame();

#ifdef TEST_MULTI_WINDOW
        for (ChildWindow& window: child_windows) {
            render_server->begin_target(window.rt);
            render_server->begin_imgui(window.rt);

            world->draw();

            ImGui::Begin("Camera");
            ImGui::DragFloat3("Position", glm::value_ptr(window.rt->transform.position), 0.01F);
            ImGui::DragFloat4("Quaternion", glm::value_ptr(window.rt->transform.quaternion), 0.01F);
            ImGui::DragFloat3("Scale", glm::value_ptr(window.rt->transform.scale), 0.01F);
            ImGui::End();

            render_server->end_imgui(window.rt);
            render_server->end_target(window.rt);
        }
#endif

        render_server->begin_target(rt_texture);

        world->draw();

        render_server->end_target(rt_texture);

        render_server->begin_target(rt_window);

#if defined(IMGUI_SUPPORT)
        render_server->begin_imgui(rt_window);
#endif

        world->draw();

        //actor->transform.position = glm::vec3(0, sin(world->elapsed_time), 0);

#if defined(IMGUI_SUPPORT)
        fps_stack.push_back(floor(1.0 / delta));

        while (fps_stack.size() > 10) {
            fps_stack.erase(fps_stack.begin());
        }

        double sum = 0;
        double count = 0;
        for (double f: fps_stack) {
            sum += f;
            count += 1;
        }

        double average = floor(sum / count);

        ImGui::Begin("Child Window Test");

        if (ImGui::Button("Create Window")) {
            window_name = "Sapphire Sub Window (";
            window_name += render_server->get_name();
            window_name += ")";

            SDL_Window* sub_window = SDL_CreateWindow(
                    window_name.c_str(),
                    SDL_WINDOWPOS_UNDEFINED,
                    SDL_WINDOWPOS_UNDEFINED,
                    800,
                    600,
                    window_flags | SDL_WINDOW_RESIZABLE
            );

            SDLWindowRenderTarget* rt_sub_window = new SDLWindowRenderTarget(sub_window);

            render_server->populate_render_target_data(rt_sub_window);

            SDL_SetWindowData(sub_window, "RT", rt_sub_window);

            render_server->initialize_imgui(rt_sub_window);

            rt_sub_window->transform.position = {1, 0, 2};
            rt_sub_window->world = world;

            child_windows.push_back({sub_window, rt_sub_window});
        }

        ImGui::End();

        float correction = -render_server->get_coordinate_correction().y;

        ImGui::Begin("Render Target");
        ImGui::DragFloat3("Position", glm::value_ptr(rt_texture->transform.position), 0.01F);
        ImGui::DragFloat4("Quaternion", glm::value_ptr(rt_texture->transform.quaternion), 0.01F);
        ImGui::DragFloat3("Scale", glm::value_ptr(rt_texture->transform.scale), 0.01F);
        ImGui::Image(rt_texture->texture->get_imgui_handle(), {256, 256}, {0, 0}, {1, correction});
        ImGui::End();

        ImGui::Begin("Panels");
        ImGui::Checkbox("World", &world_actor_panel->open);
        ImGui::Checkbox("Actor", &actor_panel->open);
        ImGui::Checkbox("Renderer", &renderer_panel->open);
        ImGui::End();


        ImGui::Begin("SEMD Debugger");
        ImGui::InputText("OBJ Path", &test_obj_path);
        ImGui::InputText("SEMD Path", &test_semd_path);
        ImGui::DragFloat3("Position", glm::value_ptr(test_position), 0.01F);

        if (ImGui::Button("Create")) {
            MeshAsset* debug_mesh = reinterpret_cast<MeshAsset*>(AssetLoader::load_asset(test_obj_path));
            MaterialAsset *debug_material = reinterpret_cast<MaterialAsset *>(AssetLoader::load_asset(test_semd_path));

            MeshActor* debug_actor = new MeshActor();
            debug_actor->mesh_asset = debug_mesh;
            debug_actor->material_asset = debug_material;

            debug_actor->transform.position = test_position;

            world->add_actor(debug_actor);
        }

        ImGui::End();

        world_actor_panel->draw_panel();
        actor_panel->draw_panel();
        renderer_panel->draw_panel();

        actor_panel->target = world_actor_panel->selected;

        //ImGui::ShowDemoWindow();
#endif

#if defined(IMGUI_SUPPORT)
        render_server->end_imgui(rt_window);
#endif

        render_server->end_target(rt_window);

        render_server->end_frame();

        render_server->present(main_window);
    }

    AssetLoader::unload_all_placeholders();

    delete actor;
    delete mesh;
    delete material;

    delete rt_window;

    delete render_server;

    return 0;
}
