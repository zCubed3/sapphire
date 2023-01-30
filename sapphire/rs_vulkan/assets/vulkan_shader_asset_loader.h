#ifndef SAPPHIRE_VULKAN_SHADER_ASSET_LOADER_H
#define SAPPHIRE_VULKAN_SHADER_ASSET_LOADER_H

#include <cstdint>
#include <vulkan/vulkan.h>
#include <vector>

#include <engine/assets/asset_loader.h>

// TODO: Geometry shader
// TODO: Move this back into the RenderServer API
class VulkanShaderAssetLoader : public AssetLoader {
public:
    std::vector<std::string> get_extensions() override;

    Asset * load_from_path(const std::string &path, const std::string& extension) override;

    void load_placeholders() override;
    void unload_placeholders() override;
};

#endif
