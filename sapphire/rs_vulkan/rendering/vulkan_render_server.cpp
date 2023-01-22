#include "vulkan_render_server.h"

#include <SDL.h>
#include <vulkan/vulkan.h>

void VulkanRenderServer::register_rs_asset_loaders() {

}

std::string VulkanRenderServer::get_name() const {
    // TODO: Include API version?
    return "Vulkan";
}

std::string VulkanRenderServer::get_error() const {
    // TODO: Vulkan render server errors
    return "";
}

uint32_t VulkanRenderServer::get_sdl_window_flags() const {
    return SDL_WINDOW_VULKAN;
}

// TODO: Vulkan render server errors
bool VulkanRenderServer::initialize(SDL_Window *p_window) {
    if (singleton != nullptr) {
        return false;
    }

    if (p_window == nullptr) {
        return false;
    }

    singleton = this;

    return true;
}

bool VulkanRenderServer::present(SDL_Window *p_window) {
    return true;
}

bool VulkanRenderServer::begin_render(RenderTarget *p_target) {
    return true;
}

bool VulkanRenderServer::end_render(RenderTarget *p_target) {
    return true;
}

void VulkanRenderServer::populate_mesh_buffer(MeshAsset *p_mesh_asset) const {
    // TODO
}