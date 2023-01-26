#ifndef SAPPHIRE_VAL_DESCRIPTOR_SET_H
#define SAPPHIRE_VAL_DESCRIPTOR_SET_H

#include <vector>
#include <vulkan/vulkan.h>

#include <rs_vulkan/val/val_releasable.h>

class ValDescriptorSet : public ValReleasable {
public:
    std::vector<VkDescriptorSetLayout> vk_descriptor_set_layouts;
    std::vector<VkDescriptorSet> vk_descriptor_sets;

    void release(ValInstance *p_val_instance) override;
};

#endif
