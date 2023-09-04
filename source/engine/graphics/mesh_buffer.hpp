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

#ifndef SAPPHIRE_MESH_BUFFER_HPP
#define SAPPHIRE_MESH_BUFFER_HPP

#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace Sapphire::Graphics {
    class MemoryBlock;
    class VulkanProvider;

    // An abstraction over mesh data inside of Vulkan
    // Automatically handles allocation and deallocation
    // Should be passed around with shared_ptr's!
    class MeshBuffer {
    protected:
        std::shared_ptr<MemoryBlock> mb_vertices;
        std::shared_ptr<MemoryBlock> mb_triangles;
        VkDeviceSize element_count;

    public:
        // TODO: User defined vertex types instead of this hard-coded type?
        struct Vertex {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 uv0;
        };

        // TODO: Allow updating the data post-creation?
        // TODO: Allow keeping the data on the CPU afterward?

        MeshBuffer() = delete;
        MeshBuffer(VulkanProvider* p_provider, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& triangles);

        // TODO: More safety around this?
        // e.g. requiring the shader has the same vertex data?
        void draw(VkCommandBuffer vk_cmd_buffer);
    };
}

#endif//SAPPHIRE_MESH_BUFFER_HPP
