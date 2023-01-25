#ifndef SAPPHIRE_VAL_STAGINGBUFFER_H
#define SAPPHIRE_VAL_STAGINGBUFFER_H

#include <cstdint>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <rs_vulkan/val/val_releasable.h>

// Wrapper around data transfers
class ValInstance;
class ValBuffer;

class ValStagingBuffer : public ValReleasable {
public:
    ValBuffer *val_staging_buffer;
    ValBuffer *val_final_buffer;

    ValStagingBuffer(size_t size, uint32_t usage_flags, ValInstance *p_val_instance);

    void write(void *data, ValInstance *p_val_instance) const;
    void copy_buffer(VkCommandBuffer vk_command_buffer) const;
    ValBuffer* finalize(ValInstance *p_val_instance);

    void release(ValInstance *p_val_instance) override;
};

#endif
