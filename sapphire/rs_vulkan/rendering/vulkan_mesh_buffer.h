#ifndef SAPPHIRE_VULKAN_MESH_BUFFER_H
#define SAPPHIRE_VULKAN_MESH_BUFFER_H

#include <engine/rendering/mesh_buffer.h>

class MeshAsset;

class VulkanMeshBuffer : public MeshBuffer {
public:
    VulkanMeshBuffer(MeshAsset* p_mesh_asset);

    void render(const Transform &transform, ShaderAsset *p_shader_asset) override;
};


#endif
