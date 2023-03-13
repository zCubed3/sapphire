#ifndef SAPPHIRE_ENGINE_HPP
#define SAPPHIRE_ENGINE_HPP

#include <core/config/config_file.hpp>

#include <engine/scene/universe.hpp>

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

#if defined(SAPPHIRE_EDITOR)
class Editor;
#endif

class Engine {
public:
    Timing *timing = nullptr;
    const Platform *platform = nullptr;
    RenderServer *render_server = nullptr;

    Universe universe;

    SDL_Window *main_window = nullptr;
    WindowRenderTarget *main_window_rt = nullptr;

    ConfigFile engine_config;

    // TODO: Temporary, lighting needs an overhaul
    Light* light = nullptr;

#if defined(SAPPHIRE_EDITOR)
    Editor* editor = nullptr;
#endif

#if defined(DEBUG)
    World* world = nullptr;
    std::vector<Actor*> actors;

    std::shared_ptr<MeshAsset> mesh = nullptr;
    std::shared_ptr<MaterialAsset> material = nullptr;
    std::shared_ptr<MeshAsset> skybox_mesh = nullptr;
    std::shared_ptr<MaterialAsset> skybox_material = nullptr;
#endif

protected:
    void initialize_configs();

    bool initialize_rendering();

    bool create_main_window();

public:
    bool initialize();
    bool shutdown();

    bool engine_loop();

    std::string get_default_window_title() const;
    void set_window_title(const std::string &title);

#if defined(DEBUG)
    bool create_test_world();
#endif
};


#endif
