#ifndef SAPPHIRE_VAL_WINDOW_H
#define SAPPHIRE_VAL_WINDOW_H

#include <vector>
#include <vulkan/vulkan.h>
#include <rs_vulkan/val/val_releasable.h>

typedef struct SDL_Window SDL_Window;

struct ValImage;
class ValInstance;

// TODO: Properly support multiple windows
// TODO: Separate this into a framebuffer type
// TODO: Align this and various other types with Val*CreateInfo structure
struct ValWindow : public ValReleasable {
    struct PresentInfo {
        VkSurfaceFormatKHR vk_color_format;
        VkSurfaceFormatKHR vk_depth_format;
        VkPresentModeKHR vk_mode;
    };

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
    ValImage* val_depth_image = nullptr;

    ValWindow(SDL_Window *p_window, VkInstance vk_instance);

    PresentInfo *get_present_info(VkPhysicalDevice vk_gpu) const;
    //bool create_render_pass(ValInstance* p_val_instance);
    bool recreate_swapchain(ValInstance* p_val_instance);

    bool begin_rendering(ValInstance* p_val_instance);
    bool end_rendering(ValInstance* p_val_instance);

    bool present_queue(ValInstance* p_val_instance);

    void release(ValInstance *p_val_instance) override;
};

#endif
