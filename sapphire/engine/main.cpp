#include <iostream>

#include <SDL.h>

#include <val/val_instance.hpp>

using namespace val;

int main(int argc, char **argv) {
    SDL_Window *sdl_window = SDL_CreateWindow(
            "Sapphire",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            1280,
            720,
            SDL_WINDOW_VULKAN
    );

    ValInstanceCreateInfo val_create_info{};

    val_create_info.engine_name = "Sapphire Engine";
    val_create_info.application_name = "Sapphire Application";

    // TODO: Optional SDL?
    // TODO: Allow switching between sRGB and Linear at runtime
    ValInstancePresentPreferences present_preferences{};
    present_preferences.use_vsync = true;

    val_create_info.p_present_preferences = &present_preferences;

    val_create_info.p_sdl_window = sdl_window;
    val_create_info.instance_extensions = ValExtension::get_sdl_instance_extensions(sdl_window);
    val_create_info.device_extensions = {{VK_KHR_SWAPCHAIN_EXTENSION_NAME, 0}};

    // TODO: Make this a commandline argument instead
#ifdef DEBUG
    val_create_info.validation_layers = {
            {"VK_LAYER_KHRONOS_validation", 0}};
#endif

    val_create_info.vk_enabled_features.fillModeNonSolid = true;

    val_create_info.requested_queues = {
            ValQueue::QueueType::QUEUE_TYPE_GRAPHICS,
            ValQueue::QueueType::QUEUE_TYPE_PRESENT,
            ValQueue::QueueType::QUEUE_TYPE_TRANSFER,
    };

    ValInstance* instance = ValInstance::create_val_instance(&val_create_info);

    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) {

        }


    }

    return 0;
}
