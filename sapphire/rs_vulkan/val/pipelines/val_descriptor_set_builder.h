#ifndef SAPPHIRE_VAL_DESCRIPTOR_SET_BUILDER_H
#define SAPPHIRE_VAL_DESCRIPTOR_SET_BUILDER_H

#include <vector>
#include <vulkan/vulkan.h>

#include <rs_vulkan/val/pipelines/val_descriptor_set.h>

class ValInstance;

class ValDescriptorSetBuilderSetInfo {
protected:
    VkDescriptorSetLayoutCreateInfo layout_info{};
    std::vector<VkDescriptorSetLayoutBinding> bindings;

public:
    void push_binding(VkDescriptorSetLayoutBinding vk_binding);
    uint32_t get_open_location(uint32_t vk_stage_flags);

    VkDescriptorSetLayout build(ValInstance *p_val_instance);
};

class ValDescriptorSetBuilder {
protected:
    std::vector<ValDescriptorSetBuilderSetInfo> sets;

public:
    void push_set();
    void push_binding(VkDescriptorType vk_type, uint32_t count, uint32_t vk_stage_flags);

    ValDescriptorSet *build(ValInstance *p_val_instance);
};


#endif
