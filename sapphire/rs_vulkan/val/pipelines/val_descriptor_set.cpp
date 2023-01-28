#include "val_descriptor_set.h"

#include <rs_vulkan/val/val_instance.h>

void ValDescriptorSet::release(ValInstance *p_val_instance) {
    ValReleasable::release(p_val_instance);

    if (vk_descriptor_set != nullptr) {
        vkFreeDescriptorSets(p_val_instance->vk_device, p_val_instance->vk_descriptor_pool, 1, &vk_descriptor_set);
        vk_descriptor_set = nullptr;
    }
}
