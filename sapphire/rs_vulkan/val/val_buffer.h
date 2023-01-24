#ifndef SAPPHIRE_VAL_BUFFER_H
#define SAPPHIRE_VAL_BUFFER_H

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

struct ValBuffer {
    VmaAllocation vma_allocation;
    VkBuffer vk_buffer;
};


#endif
