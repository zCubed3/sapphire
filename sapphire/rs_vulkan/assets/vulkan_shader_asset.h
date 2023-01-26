#ifndef SAPPHIRE_VULKAN_SHADER_ASSET_H
#define SAPPHIRE_VULKAN_SHADER_ASSET_H

#include <cstdint>
#include <vulkan/vulkan.h>
#include <vector>

#include <engine/assets/shader_asset.h>

class ValPipeline;

// TODO: Geometry shader
// TODO: Move this back into the RenderServer API
class VulkanShaderAsset : public ShaderAsset {
public:
    ValPipeline* val_pipeline = nullptr;

    // TODO: Transparency
    void create_vert_frag(const std::vector<char>& vert_code, const std::vector<char>& frag_code);

    ~VulkanShaderAsset() override;
};

#endif
