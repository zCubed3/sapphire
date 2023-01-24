#ifndef SAPPHIRE_VULKAN_MESH_BUFFER_H
#define SAPPHIRE_VULKAN_MESH_BUFFER_H

#include <engine/rendering/mesh_buffer.h>
#include <vulkan/vulkan.h>

class MeshAsset;

class VulkanMeshBuffer : public MeshBuffer {
protected:
    uint32_t tri_count;
    VkBuffer vk_buffer = nullptr;

public:
    VulkanMeshBuffer(MeshAsset* p_mesh_asset);

    void render(const Transform &transform, ShaderAsset *p_shader_asset) override;
};


#endif
