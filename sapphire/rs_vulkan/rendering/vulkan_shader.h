#ifndef SAPPHIRE_VULKAN_SHADER_H
#define SAPPHIRE_VULKAN_SHADER_H

#include <engine/rendering/shader.h>
#include <vector>

class ValPipeline;
class ValDescriptorSetInfo;

class VulkanShader : public Shader {
public:
    static VulkanShader *error_shader;

    ValPipeline* val_pipeline = nullptr;
    ValDescriptorSetInfo * val_material_descriptor_set = nullptr;
    ValDescriptorSetInfo * val_object_descriptor_set = nullptr;

    ~VulkanShader() override;

    bool make_from_semd(ConfigFile *p_semd_file) override;

    void create_vert_frag(const std::vector<char>& vert_code, const std::vector<char>& frag_code);

    static void create_error_shader();
};


#endif
