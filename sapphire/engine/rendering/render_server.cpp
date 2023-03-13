#include "render_server.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include <SDL.h>
#include <SDL_vulkan.h>

#include <core/data/size_tools.hpp>
#include <engine/assets/mesh_asset.hpp>
#include <engine/assets/texture_asset.hpp>
#include <engine/scene/world.hpp>

#include <engine/rendering/window.hpp>

#include <val/pipelines/val_pipeline.hpp>
#include <val/render_targets/val_image_render_target.h>

#if defined(IMGUI_SUPPORT)
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#endif

#ifdef DEBUG
#include <iostream>
#define RS_VULKAN_DEBUG
#endif

using namespace VAL;
using namespace Sapphire::Rendering;

// TODO: Vulkan render server errors
RenderServer::RenderServer() {
    SDL_Window *sdl_window = SDL_CreateWindow(
            "Sapphire",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            1024,
            768,
            SDL_WINDOW_VULKAN);

    ValInstanceCreateInfo val_create_info{};

    val_create_info.engine_name = "Sapphire Engine";
    val_create_info.app_name = "Sapphire Application";

    // TODO: Optional SDL?
    // TODO: Allow switching between sRGB and Linear at runtime
    ValInstancePresentPreferences present_preferences{};
    present_preferences.vsync = true;

    val_create_info.present_preferences = present_preferences;

    val_create_info.p_sdl_window = sdl_window;
    val_create_info.instance_extensions = ValExtension::get_sdl_instance_extensions(sdl_window);
    val_create_info.device_extensions = {{VK_KHR_SWAPCHAIN_EXTENSION_NAME, 0}};

#ifdef RS_VULKAN_DEBUG
    val_create_info.validation_layers = {
            {"VK_LAYER_KHRONOS_validation", 0}};
#endif

    val_create_info.vk_enabled_features.fillModeNonSolid = true;

    val_create_info.requested_queues = {
            ValQueue::QueueType::QUEUE_TYPE_GRAPHICS,
            ValQueue::QueueType::QUEUE_TYPE_PRESENT,
            ValQueue::QueueType::QUEUE_TYPE_TRANSFER,
    };

    val_instance = std::make_unique<ValInstance>(val_create_info);

    // After this, our window is sealed and can only be modified by the render server!
    auto window_ptr = std::unique_ptr<SDL_Window, std::function<void(SDL_Window *)>>(
            sdl_window,
            [](SDL_Window *window) {
                // TODO: Should this be an error?
                if (window != nullptr) {
                    SDL_DestroyWindow(window);
                }
            });

    main_window = std::make_unique<Window>(std::move(window_ptr));
}

RenderServer::~RenderServer() {

}

void RenderServer::handle_event(SDL_Event *p_event) {
    if (p_event == nullptr) {
        throw std::runtime_error("SDL event was nullptr!");
    }

    // Determine which window this event pertains to
    // TODO: Main window


    // TODO: Child windows
}

bool RenderServer::begin_frame() {
    return true;
}

bool RenderServer::end_frame() {
    return true;
}