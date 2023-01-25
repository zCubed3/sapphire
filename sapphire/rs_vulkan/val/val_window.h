#ifndef SAPPHIRE_VAL_WINDOW_H
#define SAPPHIRE_VAL_WINDOW_H

#include <vector>
#include <vulkan/vulkan.h>

typedef struct SDL_Window SDL_Window;

class ValInstance;

// TODO: Properly support multiple windows
// TODO: Separate this into a framebuffer type
struct ValWindow {
    struct PresentInfo {
        VkSurfaceFormatKHR vk_color_format;
        VkSurfaceFormatKHR vk_depth_format;
        VkPresentModeKHR vk_mode;
    };

    ValWindow(SDL_Window *p_window, VkInstance vk_instance);

    PresentInfo *get_present_info(VkPhysicalDevice vk_gpu) const;
    bool create_render_pass(ValInstance* p_val_instance);
    bool recreate_swapchain(ValInstance* p_val_instance);

    bool begin_rendering(ValInstance* p_val_instance);
    bool end_rendering(ValInstance* p_val_instance);

    bool present_queue(ValInstance* p_val_instance);

    static void release(ValWindow* p_window, ValInstance *p_val_instance);

    VkSurfaceKHR vk_surface = nullptr;
    VkSwapchainKHR vk_swapchain = nullptr;
    VkSurfaceCapabilitiesKHR vk_capabilities;
    VkExtent2D vk_extent;
    uint32_t vk_frame_index = 0;

    VkCommandBuffer vk_command_buffer = nullptr;

    SDL_Window* sdl_window;

    std::vector<VkImage> vk_swapchain_images;
    std::vector<VkImageView> vk_swapchain_image_views;
    std::vector<VkFramebuffer> vk_swapchain_framebuffers;

    // TODO: Wrap this into ValImage
    VkImage vk_depth_image = nullptr;
    VkDeviceMemory vk_depth_image_memory = nullptr;
    VkImageView vk_depth_image_view = nullptr;
};

#endif
