#include "vulkan_shader_asset_loader.h"

#include <rs_vulkan/assets/vulkan_shader_asset.h>

#include <fstream>

void VulkanShaderAssetLoader::load_placeholders() {
    VulkanShaderAsset::get_placeholder();
}

std::vector<std::string> VulkanShaderAssetLoader::get_extensions() {
    return {"spvb"};
}

Asset *VulkanShaderAssetLoader::load_from_path(const std::string &path) {
    // Verify the shader actually exists first
    // TODO: Smarter verification
    // TODO: OS utilities?
    std::ifstream file(path);

    if (file.is_open()) {
        file.close();
        return new VulkanShaderAsset(path);
    }

    return nullptr;
}