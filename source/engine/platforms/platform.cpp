#include "platform.hpp"

#include <mana/builders/mana_render_pass_builder.hpp>

#include <mana/mana_instance.hpp>
#include <mana/mana_window.hpp>
#include <mana/mana_render_context.hpp>

#include <engine/graphics/pipelines/debug_pipeline.hpp>

#include <SDL.h>

using namespace Sapphire;

int Platform::program_loop() {
    SDL_Init(SDL_INIT_EVERYTHING);

    ManaVK::ManaInstance::ManaConfig config;

    config.engine_name = "Sapphire Engine";

    config.app_name = "Sapphire Application";

    config.debugging.verbose = true;
    config.debugging.enable_khronos_layer = true;

    auto pipeline = std::make_shared<Graphics::DebugPipeline>();
    config.mana_pipeline = pipeline;

    try {
        auto instance = new ManaVK::ManaInstance(config);

        // After mana is initialized, we should tell the main window to render

        SDL_Event event;
        while (true) {
            SDL_PollEvent(&event);

            instance->flush();

            // A call to begin rendering returns a ManaRenderContext
            // This is a wrapper around both an image and a window
            {
                auto context = instance->get_main_window()->new_frame();
                pipeline->new_frame(context);
            }
        }
    } catch (std::exception& e) {
        display_os_popup("Unhandled Exception!", e.what());
    }

    /*
    Sapphire::Engine::EngineConfig config;

    try {
        Sapphire::Engine engine(config);

        bool run = true;

        while (run) {
            Engine::StepResult sr = engine.tick();

            if (sr != Engine::StepResult::Success) {
                run = false;
                break;
            }
        }
    }
    catch (std::exception e) {
        std::cout << "Encountered an unhandled exception when running the engine!\n\n" << e.what() << std::endl;
        return 1;
    }
     */

    return 0;
}