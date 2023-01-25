#ifndef SAPPHIRE_VAL_RENDER_TARGET_H
#define SAPPHIRE_VAL_RENDER_TARGET_H

#include <vulkan/vulkan.h>
#include <rs_vulkan/val/val_releasable.h>

typedef struct SDL_Window SDL_Window;

// All types of render target can be created via this info
//
// Note: Creating a window
//  1) Set p_window to a valid SDL window pointer
//  2) Set type = RENDER_TARGET_TYPE_WINDOW
struct ValRenderTargetCreateInfo {
    enum RenderTargetType {
        RENDER_TARGET_TYPE_IMAGE,
        RENDER_TARGET_TYPE_WINDOW
    };

    RenderTargetType type = RenderTargetType::RENDER_TARGET_TYPE_IMAGE;

    // If using a window this value is ignored
    VkExtent2D extent;

    // Is this depth buffered?
    bool depth_buffered = true;

#ifdef SDL_SUPPORT
    // Do we initialize the swapchain on creation?
    bool initialize_swapchain = true;

    SDL_Window *p_window = nullptr;
#endif
};

class ValRenderTarget : public ValReleasable {
protected:
    // TODO: Expose this?
    virtual VkFramebuffer get_framebuffer(ValInstance *p_val_instance) = 0;
    virtual VkExtent2D get_extent(ValInstance *p_val_instance);

    VkExtent2D creation_extent {};
    bool depth_buffered = true;

public:
    // Each render target has a command buffer
    // This is because usually a render target renders its own world
    VkCommandBuffer vk_command_buffer = nullptr;

    VkClearColorValue clear_color = {0, 0, 0, 1};
    VkClearDepthStencilValue clear_depth_stencil = {1.0F, 0};

    virtual bool begin_render(ValInstance *p_val_instance);
    virtual bool end_render(ValInstance *p_val_instance);

    static ValRenderTarget *create_render_target(ValRenderTargetCreateInfo *p_create_info, ValInstance *p_val_instance);
};

#endif
