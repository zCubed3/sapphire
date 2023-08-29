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

#include "mesh_buffer.hpp"

#include <graphics/memory_block.hpp>
#include <graphics/vulkan_provider.hpp>

#include <stdexcept>

using namespace Sapphire;

Graphics::MeshBuffer::MeshBuffer(VulkanProvider *p_provider, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &triangles) {
    if (p_provider == nullptr) {
        throw std::runtime_error("p_provider was nullptr!");
    }

    // First we're going to allocate mesh data
    // This is a combination of index and vertex data in the same block (but not necessarily adjacent)
    // TODO: Does possible fragmentation within the same buffer impact performance?
    size_t vertex_size = vertices.size() * sizeof(Vertex);
    size_t index_size = triangles.size() * sizeof(uint32_t);

    mb_vertices = p_provider->upload_memory(vertex_size, (void*)vertices.data(), VulkanProvider::AllocationType::Mesh);
    mb_triangles = p_provider->upload_memory(index_size, (void*)triangles.data(), VulkanProvider::AllocationType::Mesh);
    element_count = triangles.size();
}

void Graphics::MeshBuffer::draw(VkCommandBuffer vk_cmd_buffer) {
    // TODO: Do something other than just "not draw" when not loaded in?
    if (!mb_vertices->is_uploaded() || !mb_triangles->is_uploaded()) {
        return;
    }

    VkBuffer vertex_buffer = mb_vertices->get_vk_buffer();
    VkBuffer triangle_buffer = mb_triangles->get_vk_buffer();

    VkDeviceSize vertex_offset = mb_vertices->get_vk_offset();
    VkDeviceSize triangle_offset = mb_triangles->get_vk_offset();

    vkCmdBindVertexBuffers(vk_cmd_buffer, 0, 1, &vertex_buffer, &vertex_offset);
    vkCmdBindIndexBuffer(vk_cmd_buffer, triangle_buffer, triangle_offset, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(vk_cmd_buffer, element_count, 1, 0, 0, 0);
}
