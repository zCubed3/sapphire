#ifndef SAPPHIRE_VAL_DESCRIPTOR_SET_INFO_H
#define SAPPHIRE_VAL_DESCRIPTOR_SET_INFO_H

#include <vector>
#include <vulkan/vulkan.h>

#include <rs_vulkan/val/pipelines/val_descriptor_set.h>
#include <rs_vulkan/val/val_releasable.h>

class ValDescriptorSetInfo : public ValReleasable {
public:
    VkDescriptorSetLayout vk_descriptor_set_layout = nullptr;
    ValDescriptorSet *val_descriptor_set = nullptr;

    // Allocates a VkDescriptorSet instance from the contained layout
    ValDescriptorSet* allocate_set(ValInstance *p_val_instance);

    // Releases the set bound to this (useful if you need unique sets but want to free up pool space)
    void release_set(ValInstance *p_val_instance);

    void release(ValInstance *p_val_instance) override;
};

#endif
