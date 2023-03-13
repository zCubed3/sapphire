#ifndef SAPPHIRE_RENDER_SERVER_HPP
#define SAPPHIRE_RENDER_SERVER_HPP

#include <val/pipelines/val_descriptor_set_builder.hpp>
#include <val/pipelines/val_render_pass_builder.hpp>
#include <val/pipelines/val_vertex_input_builder.hpp>
#include <val/render_targets/val_render_target.h>
#include <val/val_instance.hpp>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <glm.hpp>

#include <vector>
#include <memory>
#include <functional>

union SDL_Event;

namespace Sapphire::Rendering {
    class Window;

    // The render server owns our primary window
    // This is because Vulkan requires 1 surface to exist at initialization time
    // Any subsequent windows are still owned by the render server, but are not destroyed alongside it
    class RenderServer {
    public:
        std::unique_ptr<Window> main_window;
        // TODO: Store list of child windows
        std::unique_ptr<VAL::ValInstance> val_instance = nullptr;

        RenderServer();
        ~RenderServer();

        void handle_event(SDL_Event* p_event);

        bool begin_frame();
        bool end_frame();

#if defined(IMGUI_SUPPORT)
        //void initialize_imgui(WindowRenderTarget *p_target) override;
        //bool begin_imgui(WindowRenderTarget *p_target) override;
        //bool end_imgui(WindowRenderTarget *p_target) override;
        //void release_imgui(WindowRenderTarget *p_target) override;
#endif

        // TODO: Make this more abstract?
        VkCommandBuffer begin_upload(bool staging = true) const;
        void end_upload(VkCommandBuffer buffer, bool staging = true) const;
    };
}

#endif
