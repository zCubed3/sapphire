#ifndef SAPPHIRE_VAL_BUFFER_H
#define SAPPHIRE_VAL_BUFFER_H

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <rs_vulkan/val/val_releasable.h>

class ValInstance;

class ValBuffer : public ValReleasable {
public:
    VmaAllocation vma_allocation;
    VkBuffer vk_buffer;
    size_t size;

    ValBuffer() = default;
    ValBuffer(size_t size, uint32_t usage, uint32_t flags, ValInstance *p_val_instance);

    void write(void *data, ValInstance *p_val_instance);

    void release(ValInstance *p_val_instance) override;
};


#endif
