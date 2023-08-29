/*
MIT License

Copyright (c) 2023 zCubed (Liam R.)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "memory_block.hpp"

#include <engine.hpp>
#include <graphics/vulkan_provider.hpp>

#include <iostream>
#include <stdexcept>

using namespace Sapphire;

//
// MemoryBlock
//
Graphics::MemoryBlock::MemoryBlock(size_t chunk_index, VkBuffer vk_parent_buffer, VmaVirtualAllocation vma_valloc, VkDeviceSize vk_offset) {
    this->chunk_index = chunk_index;
    this->vk_parent_buffer = vk_parent_buffer;
    this->vma_valloc = vma_valloc;
    this->vk_offset = vk_offset;
}

VkBuffer Graphics::MemoryBlock::get_vk_buffer() {
    return vk_parent_buffer;
}

VkDeviceSize Graphics::MemoryBlock::get_vk_offset() {
    return vk_offset;
}

size_t Graphics::MemoryBlock::get_chunk_index() {
    return chunk_index;
}

//
// MemoryPoolChunk
//
std::shared_ptr<Graphics::MemoryBlock> Graphics::MemoryPoolChunk::try_alloc(size_t size) {
    VmaVirtualAllocationCreateInfo valloc_create_info {};
    valloc_create_info.size = size;

    VmaVirtualAllocation allocation;
    VkDeviceSize offset;
    VkResult result = vmaVirtualAllocate(vma_vblock, &valloc_create_info, &allocation, &offset);

    // TODO: Locate which result enum corresponds to out of memory (to expand the pool)

    return std::make_shared<MemoryBlock>(chunk_index, vk_buffer, allocation, offset);
}

//
// MemoryPool
//
Graphics::MemoryPool::MemoryPool(Graphics::VulkanProvider *p_provider, size_t size, VkBufferUsageFlags usage, VmaAllocationCreateFlags flags) {
    this->size = size;
    this->usage = usage;
    this->flags = flags;

    push_chunk(p_provider);
}

std::shared_ptr<Graphics::MemoryBlock> Graphics::MemoryPool::alloc(size_t size) {
    // TODO: Safety here!!!!

    for (auto& chunk : chunks) {
        std::shared_ptr<MemoryBlock> block = chunk.try_alloc(size);

        if (block != nullptr) {
            return block;
        }
    }

    // TODO: Expand the pool
    throw std::runtime_error("Unable to allocate memory!!!");
}

void Graphics::MemoryPool::push_chunk(Graphics::VulkanProvider *p_provider) {
    if (p_provider == nullptr) {
        throw std::runtime_error("p_provider was nullptr!");
    }

    VmaAllocator allocator = p_provider->get_vma_allocator();
    MemoryPoolChunk chunk {};

    // First create the vblock
    // This is used by VMA to help us break a bigger VkBuffer into smaller chunks
    VmaVirtualBlockCreateInfo block_create_info{};
    block_create_info.size = size;

    VkResult result = vmaCreateVirtualBlock(&block_create_info, &chunk.vma_vblock);
    if (result != VK_SUCCESS) {
        LOG_GRAPHICS("vmaCreateVirtualBlock failed with error code (" << result << ")");
        throw std::runtime_error("vmaCreateVirtualBlock failed! Please check the log above for more info!");
    }

    // Then allocate the actual buffer
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;

    VmaAllocationCreateInfo alloc_info = {};
    alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
    alloc_info.flags = flags;

    result = vmaCreateBuffer(allocator, &buffer_info, &alloc_info, &chunk.vk_buffer, &chunk.vma_alloc, &chunk.vma_alloc_info);
    if (result != VK_SUCCESS) {
        LOG_GRAPHICS("vmaCreateBuffer failed with error code (" << result << ")");
        throw std::runtime_error("vmaCreateBuffer failed! Please check the log above for more info!");
    }

    chunk.chunk_index = chunks.size();
    chunks.push_back(chunk);
}

//
// StagingMemoryPool
//
Graphics::StagingMemoryPool::StagingMemoryPool(VulkanProvider *p_provider, size_t size, VkBufferUsageFlags usage, VmaAllocationCreateFlags flags)
    : MemoryPool(p_provider, size, usage, flags)
{
    validate_handles(p_provider);
}

void Graphics::StagingMemoryPool::push_chunk(VulkanProvider *p_provider) {
    MemoryPool::push_chunk(p_provider);
    validate_handles(p_provider);
}

void Graphics::StagingMemoryPool::validate_handles(Sapphire::Graphics::VulkanProvider *p_provider) {
    // TODO: Not be as naive and unsafe?

    VmaAllocator allocator = p_provider->get_vma_allocator();

    for (auto chunk = chunks.begin() + handles.size(); chunk != chunks.end(); chunk++) {
        void *handle = nullptr;
        vmaMapMemory(allocator, chunk->vma_alloc, &handle);

        handles.push_back((char *) handle);
    }
}

void Graphics::StagingMemoryPool::flush(VulkanProvider *p_provider) {
    // TODO: Not make begin_upload as sloppy?
    VkCommandBuffer vk_command_buffer = p_provider->begin_upload(VulkanProvider::QueueType::Transfer);

    for (auto& upload : upload_queue) {
        VkBufferCopy copy_region{};
        copy_region.srcOffset = upload.src_block->get_vk_offset();
        copy_region.dstOffset = upload.dst_block->get_vk_offset();
        copy_region.size = upload.size;

        vkCmdCopyBuffer(vk_command_buffer, upload.src_block->get_vk_buffer(), upload.dst_block->get_vk_buffer(), 1, &copy_region);
    }

    p_provider->end_upload(VulkanProvider::QueueType::Transfer, vk_command_buffer);

    // TODO: If async do this better
    // Notify all our queued upload destinations that they're now ready
    for (auto& upload : upload_queue) {
        upload.dst_block->upload_complete = true;
    }

    upload_queue.clear();
}

// TODO: Async uploading?
void Graphics::StagingMemoryPool::enqueue_upload(size_t size, void *src, std::shared_ptr<Graphics::MemoryBlock> dst) {
    // TODO: Safety
    dst->upload_complete = false;

    // Our temp block has no other purpose than to signal where our memory is going
    auto temp_block = alloc(size);

    char* staging = handles[temp_block->get_chunk_index()] + temp_block->get_vk_offset();
    memcpy(staging, src, size);

    StagedUpload upload {};
    upload.src_block = temp_block;
    upload.dst_block = dst;
    upload.size = size;

    upload_queue.push_back(upload);
}
