#ifndef SAPPHIRE_VAL_DESCRIPTOR_SET_H
#define SAPPHIRE_VAL_DESCRIPTOR_SET_H

#include <vector>
#include <vulkan/vulkan.h>

#include <rs_vulkan/val/val_releasable.h>

class ValBuffer;

// TODO: Support textures
struct ValDescriptorSetWriteInfo {
    uint32_t binding_index = 0;
    uint32_t count = 1;
    uint32_t offset = 0;

    VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ValBuffer *val_buffer = nullptr;
};

class ValDescriptorSet : public ValReleasable {
protected:
    std::vector<VkDescriptorBufferInfo*> vk_buffer_infos;
    std::vector<VkWriteDescriptorSet> vk_write_sets;

public:
    VkDescriptorSet vk_descriptor_set = nullptr;

    // Queues an update for a binding within this set
    void write_binding(ValDescriptorSetWriteInfo *p_write_info);

    // Writes to a binding within this set and the target buffer
    void write_binding_and_buffer(ValInstance *p_val_instance, ValDescriptorSetWriteInfo *p_write_info, void *data);

    // Updates the binding set
    void update_set(ValInstance *p_val_instance);

    void release(ValInstance *p_val_instance) override;
};

#endif
