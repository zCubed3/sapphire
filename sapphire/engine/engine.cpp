#include "engine.h"

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
#include <engine/rendering/lighting/light.h>
#include <engine/rendering/render_server.h>
#include <engine/rendering/shader.h>
#include <engine/rendering/texture.h>
#include <engine/rendering/texture_render_target.h>
#include <engine/rendering/window_render_target.h>
#include <engine/scene/mesh_actor.h>
#include <engine/scene/world.h>
#include <engine/timing/timing.h>
#include <engine/typing/class_registry.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/quaternion.hpp>

#ifdef RS_OPENGL4_SUPPORT
#include <rs_opengl4/rendering/opengl4_render_server.h>
#endif

#ifdef RS_VULKAN_SUPPORT
#include <rs_vulkan/rendering/vulkan_render_server.h>
#endif

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

void Engine::initialize_configs() {
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

bool Engine::initialize_rendering() {
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

std::string Engine::get_main_window_name() {
    std::string name = "Sapphire (";
    name += render_server->get_name();
    name += ")";

#ifdef DEBUG
    name += " - DEBUG BUILD";
#endif

    return name;
}

bool Engine::create_main_window() {
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

#if defined(TEST_SCENE)
bool Engine::create_test_scene() {
    mesh = std::reinterpret_pointer_cast<MeshAsset>(AssetLoader::load_asset("test.obj"));
    material = std::reinterpret_pointer_cast<MaterialAsset>(AssetLoader::load_asset("test.semd"));

    skybox_mesh = std::reinterpret_pointer_cast<MeshAsset>(AssetLoader::load_asset("test_skybox.obj"));
    skybox_material = std::reinterpret_pointer_cast<MaterialAsset>(AssetLoader::load_asset("test_skybox.semd"));

    std::shared_ptr<TextureAsset> cubemap = std::reinterpret_pointer_cast<TextureAsset>(AssetLoader::load_asset("test_cube.setd"));

    world = new World();
    world->skybox = cubemap;

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
        actor->mesh_asset = skybox_mesh;
        actor->material_asset = skybox_material;

        actors.push_back(actor);
        world->add_actor(actor);
    }

    return true;
}
#endif

#if defined(IMGUI_SUPPORT)
bool Engine::create_view_panel() {
    WorldViewPanel *view_panel = new WorldViewPanel();
    view_panel->world = world;
    view_panel->target->transform.position = {0, 0, 2};
    view_panel->target->light = light;

    view_panels.push_back(view_panel);

    return true;
}

bool Engine::draw_editor_gui() {
    ImGuiViewport *viewport = ImGui::GetMainViewport();
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

    actor_panel->target = world_actor_panel->selected;

    for (WorldViewPanel *panel: view_panels) {
        panel->draw_panel();
    }

    return true;
}

bool Engine::create_editor_panels() {
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

bool Engine::initialize() {
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

bool Engine::engine_loop() {
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

    for (WorldViewPanel *panel: view_panels) {
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