#ifndef SAPPHIRE_VULKAN_MESH_BUFFER_H
#define SAPPHIRE_VULKAN_MESH_BUFFER_H

#include <engine/rendering/buffers/mesh_buffer.h>
#include <vulkan/vulkan.h>
#include <vector>

class MeshAsset;

class ValBuffer;
class ValDescriptorSet;

class VulkanMeshBuffer : public MeshBuffer {
protected:
    uint32_t tri_count;

    ValBuffer *val_vbo = nullptr;
    ValBuffer *val_ibo = nullptr;
    ValDescriptorSet *val_object_descriptor_info = nullptr;

    // TODO: Temp
    ValDescriptorSet *val_material_descriptor_info = nullptr;

public:
    VulkanMeshBuffer(MeshAsset* p_mesh_asset);

    void render(ObjectBuffer* p_object_buffer, Shader *p_shader) override;

    ~VulkanMeshBuffer() override;
};


#endif
