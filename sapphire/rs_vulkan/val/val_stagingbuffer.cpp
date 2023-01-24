#include "val_stagingbuffer.h"

#include <rs_vulkan/val/val_instance.h>
#include <rs_vulkan/val/val_buffer.h>

ValStagingBuffer::ValStagingBuffer(size_t size, uint32_t usage_flags, ValInstance *p_val_instance) {
    VkBufferCreateInfo staging_buffer_info = {};
    staging_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    staging_buffer_info.size = size;
    staging_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VkBufferCreateInfo final_buffer_info = {};
    final_buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    final_buffer_info.size = size;
    final_buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage_flags;

    VmaAllocationCreateInfo staging_alloc_info = {};
    staging_alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
    staging_alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VmaAllocationCreateInfo final_alloc_info = {};
    final_alloc_info.usage = VMA_MEMORY_USAGE_AUTO;

    vmaCreateBuffer(p_val_instance->vma_allocator, &staging_buffer_info, &staging_alloc_info, &staging_buffer, &staging_allocation, &staging_allocation_info);
    vmaCreateBuffer(p_val_instance->vma_allocator, &final_buffer_info, &final_alloc_info, &final_buffer, &final_allocation, &final_allocation_info);

    this->size = size;
}

void ValStagingBuffer::write(void *data, ValInstance *p_val_instance) const {
    void* buffer;
    vmaMapMemory(p_val_instance->vma_allocator, staging_allocation, &buffer);
    memcpy(buffer, data, size);
    vmaUnmapMemory(p_val_instance->vma_allocator, staging_allocation);
}

void ValStagingBuffer::copy_buffer(VkCommandBuffer vk_command_buffer) const {
    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = size;

    vkCmdCopyBuffer(vk_command_buffer, staging_buffer, final_buffer, 1, &copy_region);
}

ValBuffer* ValStagingBuffer::finalize(ValStagingBuffer *p_buffer, ValInstance *p_val_instance) {
    vmaDestroyBuffer(p_val_instance->vma_allocator, p_buffer->staging_buffer, p_buffer->staging_allocation);

    ValBuffer* buffer = new ValBuffer();
    buffer->vk_buffer = p_buffer->final_buffer;
    buffer->vma_allocation = p_buffer->final_allocation;

    return buffer;
}