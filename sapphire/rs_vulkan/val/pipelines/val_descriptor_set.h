#ifndef SAPPHIRE_VAL_DESCRIPTOR_SET_H
#define SAPPHIRE_VAL_DESCRIPTOR_SET_H

#include <vector>
#include <vulkan/vulkan.h>

#include <rs_vulkan/val/val_releasable.h>

class ValDescriptorSet : public ValReleasable {
public:
    VkDescriptorSet vk_descriptor_set = nullptr;

    void release(ValInstance *p_val_instance) override;
};

#endif
