#ifndef SAPPHIRE_VULKAN_MATERIAL_H
#define SAPPHIRE_VULKAN_MATERIAL_H

#include <engine/rendering/material.h>

class ValDescriptorSet;

class VulkanMaterial : public Material {
public:
    ValDescriptorSet *val_material_descriptor_info = nullptr;

    ~VulkanMaterial() override;

    ShaderPass *bind_pass(const std::string& pass_name) override;
};


#endif
