#include "val_window.h"

#include <SDL.h>
#include <SDL_vulkan.h>

ValWindow::ValWindow(SDL_Window *p_window, VkInstance vk_instance) {
    SDL_Vulkan_CreateSurface(p_window, vk_instance, &vk_surface);
}