#include "val_descriptor_set.h"

#include <rs_vulkan/val/val_instance.h>

void ValDescriptorSet::release(ValInstance *p_val_instance) {
    ValReleasable::release(p_val_instance);

    for (VkDescriptorSetLayout &vk_descriptor_set_layout: vk_descriptor_set_layouts) {
        vkDestroyDescriptorSetLayout(p_val_instance->vk_device, vk_descriptor_set_layout, nullptr);
    }

    uint32_t count = static_cast<uint32_t>(vk_descriptor_sets.size());
    vkFreeDescriptorSets(p_val_instance->vk_device, p_val_instance->vk_descriptor_pool, count, vk_descriptor_sets.data());
}
