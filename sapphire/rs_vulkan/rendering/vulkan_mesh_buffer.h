#ifndef SAPPHIRE_VULKAN_MESH_BUFFER_H
#define SAPPHIRE_VULKAN_MESH_BUFFER_H

#include <engine/rendering/mesh_buffer.h>
#include <vulkan/vulkan.h>

class MeshAsset;

struct ValBuffer;

class VulkanMeshBuffer : public MeshBuffer {
protected:
    uint32_t tri_count;

    ValBuffer* val_vbo = nullptr;
    ValBuffer* val_ibo = nullptr;

public:
    VulkanMeshBuffer(MeshAsset* p_mesh_asset);

    void render(const Transform &transform, ShaderAsset *p_shader_asset) override;
};


#endif
