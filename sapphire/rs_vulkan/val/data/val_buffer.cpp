#include "val_buffer.h"

#include <rs_vulkan/val/val_instance.h>

ValBuffer::ValBuffer(size_t size, uint32_t usage, uint32_t flags, ValInstance *p_val_instance) {
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;

    VmaAllocationCreateInfo alloc_info = {};
    alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
    alloc_info.flags = flags;

    VmaAllocationInfo allocation_info {};
    vmaCreateBuffer(p_val_instance->vma_allocator, &buffer_info, &alloc_info, &vk_buffer, &vma_allocation, &allocation_info);

    this->size = size;
}

void ValBuffer::write(void *data, ValInstance *p_val_instance, size_t offset, size_t size) {
    size_t actual = size;
    if (size == -1) {
        actual = this->size;
    }

    if (mapped == nullptr) {
        vmaMapMemory(p_val_instance->vma_allocator, vma_allocation, reinterpret_cast<void**>(&mapped));
    }

    memcpy(mapped + offset, data, actual);
}

void ValBuffer::release(ValInstance *p_val_instance) {
    ValReleasable::release(p_val_instance);

    if (mapped != nullptr) {
        vmaUnmapMemory(p_val_instance->vma_allocator, vma_allocation);
        mapped = nullptr;
    }

    if (vma_allocation != nullptr) {
        vmaDestroyBuffer(p_val_instance->vma_allocator, vk_buffer, vma_allocation);
        vk_buffer = nullptr;
        vma_allocation = nullptr;
    }
}