#ifndef SAPPHIRE_VULKAN_SHADER_H
#define SAPPHIRE_VULKAN_SHADER_H

#include <engine/rendering/shader.h>
#include <vector>
#include <vulkan/vulkan.h>

class ValPipeline;
class ValDescriptorSetInfo;

class VulkanShader : public Shader {
protected:
    struct VulkanParameter {
        uint32_t location;
        VkDescriptorType type;
        uint32_t stage_flags;
    };

    std::vector<VulkanParameter> vulkan_parameters;

public:
    static VulkanShader *error_shader;

    ValPipeline* val_pipeline = nullptr;
    ValDescriptorSetInfo * val_material_descriptor_set = nullptr;
    ValDescriptorSetInfo * val_object_descriptor_set = nullptr;

    ~VulkanShader() override;

    bool make_from_sesd(ConfigFile *p_sesd_file) override;

    void create_vert_frag(const std::vector<char>& vert_code, const std::vector<char>& frag_code);

    static void create_error_shader();
};


#endif
