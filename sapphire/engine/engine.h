#ifndef SAPPHIRE_ENGINE_H
#define SAPPHIRE_ENGINE_H

#include <engine/config/config_file.h>

#include <vector>
#include <memory>

#define TEST_SCENE

typedef struct SDL_Window SDL_Window;

class RenderServer;
class World;
class Actor;
class MeshActor;
class Light;
class Timing;
class Platform;
class WindowRenderTarget;
class Asset;
class MeshAsset;
class MaterialAsset;

#if defined(IMGUI_SUPPORT)
class ActorPanel;
class WorldActorPanel;
class WorldViewPanel;
class RendererPanel;
class ConsolePanel;
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
    void initialize_configs();

    bool initialize_rendering();

    std::string get_main_window_name();

    bool create_main_window();

#if defined(TEST_SCENE)
    bool create_test_scene();
#endif

#if defined(IMGUI_SUPPORT)
    bool create_view_panel();

    bool draw_editor_gui();

    bool create_editor_panels();
#endif

public:
    bool initialize();

    bool engine_loop();
};


#endif
