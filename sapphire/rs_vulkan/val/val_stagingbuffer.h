#ifndef SAPPHIRE_VAL_STAGINGBUFFER_H
#define SAPPHIRE_VAL_STAGINGBUFFER_H

#include <cstdint>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

// Wrapper around data transfers
class ValInstance;
struct ValBuffer;

struct ValStagingBuffer {
    VmaAllocationInfo staging_allocation_info;
    VmaAllocation staging_allocation;
    VkBuffer staging_buffer;

    VmaAllocationInfo final_allocation_info;
    VmaAllocation final_allocation;
    VkBuffer final_buffer;

    size_t size;

    ValStagingBuffer(size_t size, uint32_t usage_flags, ValInstance *p_val_instance);

    void write(void *data, ValInstance *p_val_instance) const;
    void copy_buffer(VkCommandBuffer vk_command_buffer) const;

    static ValBuffer *finalize(ValStagingBuffer *p_buffer, ValInstance *p_val_instance);
};

#endif
