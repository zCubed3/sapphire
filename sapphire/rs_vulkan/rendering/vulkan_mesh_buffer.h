#ifndef SAPPHIRE_VULKAN_MESH_BUFFER_H
#define SAPPHIRE_VULKAN_MESH_BUFFER_H

#include <engine/rendering/mesh_buffer.h>
#include <vulkan/vulkan.h>
#include <vector>

class MeshAsset;

class ValBuffer;

class VulkanMeshBuffer : public MeshBuffer {
protected:
    struct ObjectData {
        glm::mat4 model_view_projection;
        glm::mat4 model;
        glm::mat4 model_inverse;
        glm::mat4 model_inverse_transpose;
    };
    uint32_t tri_count;

    ValBuffer *val_vbo = nullptr;
    ValBuffer *val_ibo = nullptr;
    ValBuffer *transform_ubo = nullptr;
    std::vector<VkDescriptorSet> vk_descriptor_sets;

public:
    VulkanMeshBuffer(MeshAsset* p_mesh_asset);

    void render(const Transform &transform, ShaderAsset *p_shader_asset) override;

    ~VulkanMeshBuffer() override;
};


#endif
