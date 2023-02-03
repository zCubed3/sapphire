#include <iostream>

#include <SDL.h>

#ifdef WIN32
#include <engine/platforms/win32_platform.h>
#endif

#include <engine/assets/asset_loader.h>
#include <engine/assets/material_asset.h>
#include <engine/assets/static_mesh_asset.h>
#include <engine/assets/texture_asset.h>
#include <engine/console/console.h>
#include <engine/data/string_tools.h>
#include <engine/rendering/render_server.h>
#include <engine/rendering/shader.h>
#include <engine/rendering/texture.h>
#include <engine/rendering/texture_render_target.h>
#include <engine/rendering/window_render_target.h>
#include <engine/rendering/lighting/light.h>
#include <engine/scene/mesh_actor.h>
#include <engine/scene/world.h>
#include <engine/timing/timing.h>
#include <engine/typing/class_registry.h>

#ifdef RS_OPENGL4_SUPPORT
#include <rs_opengl4/rendering/opengl4_render_server.h>
#endif

#ifdef RS_VULKAN_SUPPORT
#include <rs_vulkan/rendering/vulkan_render_server.h>
#endif

#define TEST_MULTI_WINDOW
#define TEST_SCENE

#if defined(IMGUI_SUPPORT)
#include <backends/imgui_impl_sdl.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <engine/editor/panels/actor_panel.h>
#include <engine/editor/panels/console_panel.h>
#include <engine/editor/panels/renderer_panel.h>
#include <engine/editor/panels/world_actor_panel.h>
#include <engine/editor/panels/world_view_panel.h>
#endif

#include <config/config_file.h>

#include <memory>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/quaternion.hpp>

#ifdef TEST_MULTI_WINDOW
struct ChildWindow {
    SDL_Window *window;
    WindowRenderTarget *rt;
};
#endif

class Engine {
public:
    Timing *timing = nullptr;
    const Platform *platform = nullptr;
    RenderServer *render_server = nullptr;

    SDL_Window *main_window = nullptr;
    WindowRenderTarget *main_window_rt = nullptr;

    ConfigFile engine_config;
    ConfigFile editor_config;

    // TODO: Temporary, lighting needs an overhaul
    Light* light = nullptr;

#if defined(TEST_SCENE)
    World* world = nullptr;
    std::vector<Actor*> actors;

    std::shared_ptr<MeshAsset> mesh = nullptr;
    std::shared_ptr<MeshAsset> mesh2 = nullptr;
    std::shared_ptr<MaterialAsset> material = nullptr;
#endif

#if defined(IMGUI_SUPPORT)
    WorldActorPanel* world_actor_panel = nullptr;
    ActorPanel* actor_panel = nullptr;
    RendererPanel * renderer_panel = nullptr;
    ConsolePanel* console_panel = nullptr;

    std::vector<WorldViewPanel*> view_panels;
#endif

protected:
    void initialize_configs() {
        platform->create_folder("config");

        //
        // Engine config
        //
        engine_config.set_string("sRenderServer", "Rendering", "Vulkan");

        engine_config.read_from_path("config/engine.secf");
        engine_config.write_to_path("config/engine.secf");

        //
        // Editor config
        //
        editor_config.read_from_path("config/editor.secf");
        editor_config.write_to_path("config/editor.secf");
    }

    bool initialize_rendering() {
        std::string user_render_server = engine_config.try_get_string("sRenderServer", "Rendering", "Vulkan");

        if (StringTools::compare(user_render_server, "vulkan")) {
#ifdef RS_VULKAN_SUPPORT
            render_server = new VulkanRenderServer();
            return true;
#else
            std::cout << "Error: Vulkan support was not compiled into the engine!" << std::endl;
#endif
        }

        if (StringTools::compare(user_render_server, "opengl4")) {
#ifdef RS_OPENGL4_SUPPORT
            render_server = new OpenGL4RenderServer();
            return true;
#else
            std::cout << "Error: OpenGL 4 support was not compiled into the engine!" << std::endl;
#endif
        }

        if (render_server == nullptr) {
            std::cout << "Error: Unrecognized render server '" << user_render_server << "'!" << std::endl;
            return false;
        }

        return false;
    }

    std::string get_main_window_name() {
        std::string name = "Sapphire (";
        name += render_server->get_name();
        name += ")";

#ifdef DEBUG
        name += " - DEBUG BUILD";
#endif

        return name;
    }

    bool create_main_window() {
        std::string main_window_name = get_main_window_name();

        main_window = SDL_CreateWindow(
                main_window_name.c_str(),
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                1280,
                720,
                render_server->get_sdl_window_flags() | SDL_WINDOW_RESIZABLE);

        main_window_rt = new WindowRenderTarget(main_window);
        SDL_SetWindowData(main_window, "RT", main_window_rt);

        SDL_MaximizeWindow(main_window);

        if (!render_server->initialize(main_window)) {
            std::cout << render_server->get_error() << std::endl;
            return false;
        }

        render_server->populate_render_target_data(main_window_rt);

#if defined(IMGUI_SUPPORT)
        render_server->initialize_imgui(main_window_rt);
#endif

        return true;
    }

    bool create_test_scene() {
        mesh = std::reinterpret_pointer_cast<MeshAsset>(AssetLoader::load_asset("test.obj"));
        mesh2 = std::reinterpret_pointer_cast<MeshAsset>(AssetLoader::load_asset("test2.obj"));
        material = std::reinterpret_pointer_cast<MaterialAsset>(AssetLoader::load_asset("test.semd"));

        world = new World();

        light = new Light();

        {
            MeshActor *actor = new MeshActor();
            actor->mesh_asset = mesh;
            actor->material_asset = material;

            actors.push_back(actor);
            world->add_actor(actor);
        }

        {
            MeshActor *actor = new MeshActor();
            actor->mesh_asset = mesh2;
            actor->material_asset = material;

            actors.push_back(actor);
            world->add_actor(actor);
        }

        return true;
    }

#if defined(IMGUI_SUPPORT)
    bool create_view_panel() {
        WorldViewPanel *view_panel = new WorldViewPanel();
        view_panel->world = world;
        view_panel->target->transform.position = {0, 0, 2};
        view_panel->target->light = light;

        view_panels.push_back(view_panel);

        return true;
    }

    bool draw_editor_gui() {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGuiID dockspace_id = ImGui::GetID("EditorDockspace");

        int imgui_window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        imgui_window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        imgui_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("Editor", nullptr, imgui_window_flags);
        ImGui::PopStyleVar(3);

        ImGui::BeginMenuBar();

        if (ImGui::BeginMenu("Panels")) {
            ImGui::Checkbox("World", &world_actor_panel->open);
            ImGui::Checkbox("Actor", &actor_panel->open);
            ImGui::Checkbox("Renderer", &renderer_panel->open);

            if (ImGui::Button("Create WorldViewPanel")) {
                create_view_panel();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();

        ImGui::DockSpace(dockspace_id);
        ImGui::End();

        world_actor_panel->draw_panel();
        actor_panel->draw_panel();
        renderer_panel->draw_panel();
        console_panel->draw_panel();

        for (WorldViewPanel* panel: view_panels) {
            panel->draw_panel();
        }

        return true;
    }

    bool create_editor_panels() {
        world_actor_panel = new WorldActorPanel();
        world_actor_panel->target = world;

        actor_panel = new ActorPanel();
        actor_panel->world = world;

        renderer_panel = new RendererPanel();

        console_panel = new ConsolePanel();

        create_view_panel();

        return true;
    }
#endif

public:
    bool initialize() {
#ifdef WIN32
        platform = Win32Platform::create_win32_platform();
#endif

        timing = Timing::get_singleton();

        AssetLoader::register_engine_asset_loaders();
        Console::register_defaults();

        initialize_configs();

        SDL_Init(SDL_INIT_EVERYTHING);

        if (!initialize_rendering()) {
            return false;
        }

        render_server->register_rs_asset_loaders();

        create_main_window();

        AssetLoader::load_all_placeholders();

#if defined(TEST_SCENE)
        create_test_scene();
#endif

#if defined(IMGUI_SUPPORT)
        create_editor_panels();
#endif

        return true;
    }

    bool engine_loop() {
        if (render_server == nullptr) {
            return false;
        }

        timing->new_frame();

        // TODO: Update the world properly
        float delta = static_cast<float>(timing->get_delta());

        world->delta_time = delta;
        world->elapsed_time += delta;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
#if defined(IMGUI_SUPPORT)
            if (SDL_GetWindowFlags(main_window) & SDL_WINDOW_INPUT_FOCUS) {
                ImGui::SetCurrentContext(main_window_rt->imgui_context);
                ImGui_ImplSDL2_ProcessEvent(&event);
            }
#endif

            if (event.type == SDL_QUIT) {
                return false;
            }

            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                render_server->on_window_resized(SDL_GetWindowFromID(event.window.windowID));
            }
        }

        render_server->begin_frame();

        // TODO: Temporary lighting
        light->render_shadows(render_server, world);

        for (WorldViewPanel* panel: view_panels) {
            panel->draw_world(render_server);
        }

        render_server->begin_target(main_window_rt);
        render_server->begin_imgui(main_window_rt);

        draw_editor_gui();

        render_server->end_imgui(main_window_rt);
        render_server->end_target(main_window_rt);

        render_server->end_frame();

        render_server->present(main_window);

        return true;
    }
};

int main(int argc, char **argv) {
    Engine engine;
    engine.initialize();

    while (engine.engine_loop()) {

    }

    // ImGui my beloved :)

    /*
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
            for (ChildWindow &window: child_windows) {
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
                    for (ChildWindow &window: child_windows) {
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

        world->delta_time = delta;
world->elapsed_time += delta;

        glm::vec3 euler{};
        euler.x = sin(world->elapsed_time) * 10;
        euler.y = cos(world->elapsed_time) * 10;

        world->delta_time = delta;
        world->elapsed_time += delta;

        render_server->begin_frame();

        light->render_shadows(render_server, world);

#ifdef TEST_MULTI_WINDOW
        for (ChildWindow &window: child_windows) {
            window.rt->light = light;

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

#if defined(IMGUI_SUPPORT)
        for (WorldViewPanel* panel: world_panels) {
            panel->draw_world(render_server);
        }
#endif

        render_server->begin_target(rt_window);

#if defined(IMGUI_SUPPORT)
        render_server->begin_imgui(rt_window);
#endif

        //world->draw();

        //actor->transform.position = glm::vec3(0, sin(world->elapsed_time), 0);

#if defined(IMGUI_SUPPORT)
        ImGui::Begin("WindowTester");

        if (ImGui::Button("Create Window")) {
            window_name = "Sapphire Sub Window (";
            window_name += render_server->get_name();
            window_name += ")";

            SDL_Window *sub_window = SDL_CreateWindow(
                    window_name.c_str(),
                    SDL_WINDOWPOS_UNDEFINED,
                    SDL_WINDOWPOS_UNDEFINED,
                    800,
                    600,
                    window_flags | SDL_WINDOW_RESIZABLE);

            WindowRenderTarget *rt_sub_window = new WindowRenderTarget(sub_window);

            render_server->populate_render_target_data(rt_sub_window);

            SDL_SetWindowData(sub_window, "RT", rt_sub_window);

            render_server->initialize_imgui(rt_sub_window);

            rt_sub_window->transform.position = {1, 0, 2};
            rt_sub_window->world = world;

            child_windows.push_back({sub_window, rt_sub_window});
        }

        ImGui::End();

        ImGui::Begin("SEMD Debugger");
        ImGui::InputText("OBJ Path", &test_obj_path);
        ImGui::InputText("SEMD Path", &test_semd_path);
        ImGui::DragFloat3("Position", glm::value_ptr(test_position), 0.01F);

        if (ImGui::Button("Create")) {
            std::shared_ptr<MeshAsset> debug_mesh = std::reinterpret_pointer_cast<MeshAsset>(AssetLoader::load_asset(test_obj_path));
            std::shared_ptr<MaterialAsset> debug_mat = std::reinterpret_pointer_cast<MaterialAsset>(AssetLoader::load_asset(test_semd_path));

            MeshActor *debug_actor = new MeshActor();
            debug_actor->mesh_asset = debug_mesh;
            debug_actor->material_asset = debug_mat;

            debug_actor->transform.position = test_position;

            world->add_actor(debug_actor);
        }

        ImGui::End();

        ImGui::Begin("Light Debug");
        ImGui::DragFloat3("Position", glm::value_ptr(light->sun_pos));
        ImGui::End();

        ImGui::ShowDemoWindow();

        world_actor_panel->draw_panel();
        actor_panel->draw_panel();
        renderer_panel->draw_panel();
        console_panel->draw_panel();

        size_t index = 0;
        for (WorldViewPanel* panel: world_panels) {
            if (!panel->open) {
                delete panel;
                world_panels.erase(world_panels.begin() + index);

                break;
            }

            panel->draw_panel();
            index++;
        }

        actor_panel->target = world_actor_panel->selected;

        //shadow_actor->transform = actor->transform;
        //shadow_actor2->transform = actor2->transform;
#endif

#if defined(IMGUI_SUPPORT)
        render_server->end_imgui(rt_window);
#endif

        render_server->end_target(rt_window);

        render_server->end_frame();

        render_server->present(main_window);
    }

    AssetLoader::unload_all_assets();

    delete actor;
    delete actor2;
    //delete shadow_actor;
    //delete shadow_actor2;

#if defined(IMGUI_SUPPORT)
    for (WorldViewPanel* panel: world_panels) {
        delete panel;
    }

    render_server->release_imgui(rt_window);
#endif

    delete light;
    delete rt_window;

    delete render_server;

    // To keep the user happy we store the last state of the window
    int width;
    int height;
    SDL_GetWindowSize(main_window, &width, &height);

    int x;
    int y;
    SDL_GetWindowPosition(main_window, &x, &y);

    engine_config.set_string("iMainWindowWidth", "MainWindow", std::to_string(width));
    engine_config.set_string("iMainWindowHeight", "MainWindow", std::to_string(height));
    engine_config.set_string("iMainWindowX", "MainWindow", std::to_string(x));
    engine_config.set_string("iMainWindowY", "MainWindow", std::to_string(y));

    engine_config.write_to_path("engine.secf");

     */
    return 0;
}
