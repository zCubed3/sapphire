#ifndef SAPPHIRE_VULKAN_RENDER_SERVER_H
#define SAPPHIRE_VULKAN_RENDER_SERVER_H

#include <engine/rendering/render_server.h>

#include <vulkan/vulkan.h>

#include <vector>

class VulkanRenderServer : public RenderServer {
protected:
    SDL_Window* window;

    VkInstance vk_instance = nullptr;
    VkPhysicalDevice vk_physical_device = nullptr;
    VkDevice vk_device = nullptr;

    // TODO: Other queue families?
    uint32_t vk_graphics_family;
    uint32_t vk_present_family;
    VkQueue vk_graphics_queue = nullptr;
    VkQueue vk_present_queue = nullptr;

    VkSurfaceKHR vk_surface = nullptr;
    VkSwapchainKHR vk_swapchain = nullptr;

    std::vector<VkImage> vk_swapchain_images;
    std::vector<VkImageView> vk_swapchain_image_views;
    std::vector<VkFramebuffer> vk_swapchain_framebuffers;

    VkSurfaceCapabilitiesKHR vk_capabilities;
    VkSurfaceFormatKHR vk_chosen_format;
    VkPresentModeKHR vk_chosen_present_mode;
    VkExtent2D vk_extent;

    // TODO: Allow the user to create their own render passes?
    VkRenderPass vk_render_pass = nullptr;
    VkCommandPool vk_command_pool = nullptr;
    VkCommandBuffer vk_command_buffer = nullptr;

    uint32_t vk_frame_index = 0;
    VkSemaphore vk_image_available_semaphore;
    VkSemaphore vk_render_finished_semaphore;
    VkFence vk_flight_fence;

    // Whenever the window is resized we need to recreate the swapchain entirely
    bool recreate_swapchain();

    // Waits for the frame to finish rendering continuing execution
    void await_frame();

public:
    ~VulkanRenderServer() override;

    void register_rs_asset_loaders() override;

    std::string get_name() const override;
    std::string get_error() const override;
    uint32_t get_sdl_window_flags() const override;

    bool initialize(SDL_Window *p_window) override;

    bool present(SDL_Window *p_window) override;

    void on_window_resized() override;

    bool begin_frame() override;
    bool end_frame() override;

    bool begin_target(RenderTarget *p_target) override;
    bool end_target(RenderTarget *p_target) override;

    void populate_mesh_buffer(MeshAsset *p_mesh_asset) const override;
};

#endif
