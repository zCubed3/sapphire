#include "val_extension.h"

#include <SDL.h>
#include <SDL_vulkan.h>

std::vector<ValExtension> ValExtension::get_sdl_instance_extensions(SDL_Window* p_window) {
    uint32_t count = 0;
    std::vector<const char*> extension_names;

    SDL_Vulkan_GetInstanceExtensions(p_window, &count, nullptr);
    extension_names.resize(count);
    SDL_Vulkan_GetInstanceExtensions(p_window, &count, extension_names.data());

    std::vector<ValExtension> extensions;
    for (const char* name : extension_names) {
        ValExtension extension {};
        extension.name = name;
        extension.flags = 0;

        extensions.push_back(extension);
    }

    return extensions;
}
