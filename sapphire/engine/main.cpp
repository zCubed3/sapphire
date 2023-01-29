#include <iostream>

#include <SDL.h>

#include <engine/assets/asset_loader.h>
#include <engine/assets/shader_asset.h>
#include <engine/assets/static_mesh_asset.h>
#include <engine/rendering/sdl_window_render_target.h>
#include <engine/scene/world.h>
#include <engine/typing/class_registry.h>

#ifdef RS_OPENGL4_SUPPORT
#include <rs_opengl4/rendering/opengl4_render_server.h>
#endif

#ifdef RS_VULKAN_SUPPORT
#include <rs_vulkan/rendering/vulkan_render_server.h>
#endif

#include <imgui.h>
#include <backends/imgui_impl_sdl.h>

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
    ClassRegistry::register_class<BaseClass>();
    ClassRegistry::register_class<ChildClass>();
    ClassRegistry::register_class<ChildChildClass>();

    ChildChildClass *instance = new ChildChildClass();
    bool is_child = instance->is_child_of<BaseClass>();

    BaseClass* parent_instance = ClassRegistry::as<BaseClass>(instance);
    float data = parent_instance->get_data();

    AssetLoader::register_engine_asset_loaders();

    SDL_Init(SDL_INIT_EVERYTHING);

    // The render server requires a main window to get things started
    // TODO: Splash screen?
    // TODO: Allow changing the server based on a cvar
    //RenderServer *render_server = new OpenGL4RenderServer();
    RenderServer *render_server = new VulkanRenderServer();
    render_server->register_rs_asset_loaders();

    uint32_t window_flags = render_server->get_sdl_window_flags();

    std::string window_name = "Sapphire";

    window_name += " (";
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

    // Each render target has clear operations associated with it
    rt_window->clear_flags = RenderTarget::CLEAR_FLAG_DEPTH | RenderTarget::CLEAR_FLAG_DEPTH;

    if (!render_server->initialize(main_window)) {
        std::cout << render_server->get_error() << std::endl;
        return 1;
    }

    // We need to load our model and our shader
    MeshAsset *mesh = static_cast<MeshAsset *>(AssetLoader::load_asset("test.obj"));
    //MeshAsset *mesh2 = StaticMeshAsset::get_primitive(StaticMeshAsset::PRIMITIVE_QUAD);
    MeshAsset *mesh2 = static_cast<MeshAsset *>(AssetLoader::load_asset("test2.obj"));

    ShaderAsset *shader = static_cast<ShaderAsset *>(AssetLoader::load_asset("test.mspv"));
    //ShaderAsset *shader = static_cast<ShaderAsset *>(AssetLoader::load_asset("test.glsl"));

    AssetLoader::load_all_placeholders();

    render_server->populate_render_target_data(rt_window);

    World *world = new World();

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


        last_tick = tick;

        world->elapsed_time += delta;
        world->delta_time = delta;

        glm::vec3 euler = glm::vec3(0, world->elapsed_time * 90, 0);

        float x = sin(world->elapsed_time);
        float y = cos(world->elapsed_time);

        euler.x = x * 30;
        euler.y = y * 30;

        //rt_window.clear_color = Color(abs(sin(world->elapsed_time)), 0, 0, 1);

        model.position = glm::vec3(-1, 0, 0);
        model.quaternion = glm::quat(glm::radians(euler));

        model.calculate_matrices();

        model2.position = glm::vec3(1, 0, 0);
        model2.quaternion = glm::quat(glm::radians(euler));

        model2.calculate_matrices();

        // We don't have a camera, so we need to move our render target
        rt_window->fov = 105;
        rt_window->transform.position = glm::vec3(0, 0, 2);

        render_server->begin_frame();
        render_server->begin_target(rt_window);
        render_server->begin_imgui();

        // TODO: MeshRenderer
        mesh->shader = shader;
        mesh->render(model);

        //mesh2->shader = shader;
        mesh2->render(model2);

        ImGui::Begin("Test");

        ImGui::End();

        ImGui::ShowDemoWindow();

        render_server->end_imgui();
        render_server->end_target(rt_window);
        render_server->end_frame();

        render_server->present(main_window);
    }

    AssetLoader::unload_all_placeholders();

    delete mesh;
    delete mesh2;
    delete shader;

    delete rt_window;

    delete render_server;
    ImGui::DestroyContext(imgui_context);

    return 0;
}
