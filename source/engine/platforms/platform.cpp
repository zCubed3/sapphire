#include "platform.hpp"

#include <mana/mana_instance.hpp>

using namespace Sapphire;

int Platform::program_loop() {
    // TODO: TEMP
    ManaVK::ManaInstance::ManaConfig config;

    config.debugging.verbose = true;
    config.debugging.enable_khronos_layer = true;

    try {
        ManaVK::ManaInstance *instance = new ManaVK::ManaInstance(config);
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