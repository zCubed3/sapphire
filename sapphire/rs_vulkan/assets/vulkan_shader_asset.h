#ifndef SAPPHIRE_VULKAN_SHADER_ASSET_H
#define SAPPHIRE_VULKAN_SHADER_ASSET_H

#include <cstdint>
#include <vulkan/vulkan.h>
#include <vector>

#include <engine/assets/shader_asset.h>

// TODO: Geometry shader
// TODO: Move this back into the RenderServer API
class VulkanShaderAsset : public ShaderAsset {
public:
    enum BlendState {
        Zero,
        One,

    };

    // TODO: Actually make the dynamic state flags matter
    enum DynamicStateFlags {
        Viewport = 1,
        Scissor = 2
    };

    enum CullMode {
        Back,
        Front,
        Off
    };

    BlendState source_blend = BlendState::One;
    BlendState target_blend = BlendState::Zero;

    CullMode cull_mode = CullMode::Back;
    uint32_t dynamic_flags = DynamicStateFlags::Scissor | DynamicStateFlags::Viewport;

    VkPipeline vk_pipeline = nullptr;
    VkPipelineLayout vk_pipeline_layout = nullptr;
    //VkDescriptorSetLayout vk_descriptor_set_layout = nullptr;

protected:
    enum Stage {
        Vertex,
        Fragment
    };

    static bool create_module(const std::vector<char>& code, VkShaderModule* p_module);
    static VkPipelineShaderStageCreateInfo create_stage(Stage stage, VkShaderModule p_module);

    static std::vector<VkDynamicState> get_dynamic_states(uint32_t flags);

public:
    // TODO: Transparency
    void create_vert_frag(const std::vector<char>& vert_code, const std::vector<char>& frag_code);

    ~VulkanShaderAsset() override;
};

#endif
