#ifndef SAPPHIRE_VAL_WINDOW_H
#define SAPPHIRE_VAL_WINDOW_H

#include <vector>
#include <vulkan/vulkan.h>

typedef struct SDL_Window SDL_Window;

// TODO: Properly support multiple windows
struct ValWindow {
    ValWindow(SDL_Window *p_window, VkInstance vk_instance);

    VkSurfaceKHR vk_surface = nullptr;
    VkSwapchainKHR vk_swapchain = nullptr;

    std::vector<VkImage> vk_swapchain_images;
    std::vector<VkImageView> vk_swapchain_image_views;
    std::vector<VkFramebuffer> vk_swapchain_framebuffers;
};

#endif
