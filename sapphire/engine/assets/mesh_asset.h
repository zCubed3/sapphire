#ifndef AGE_MESH_ASSET_H
#define AGE_MESH_ASSET_H

#include <engine/assets/asset.h>
#include <engine/data/transform.h>

#include <glm.hpp>

class ShaderAsset;
class MeshBuffer;

class MeshAsset : public Asset {
public:
    // TODO: Asset safety so dangling pointers can't happen
    ShaderAsset *shader = nullptr;
    MeshBuffer *buffer = nullptr;

    virtual glm::vec3 *get_position_data(size_t *p_length) = 0;
    virtual glm::vec3 *get_normal_data(size_t *p_length) = 0;
    virtual glm::vec2 *get_uv0_data(size_t *p_length) = 0;
    virtual glm::vec4 *get_tangent_data(size_t *p_length) = 0;
    virtual uint32_t *get_triangle_data(size_t *p_length) = 0;

    virtual uint32_t get_vertex_count() = 0;
    virtual uint32_t get_triangle_count() = 0;

    virtual void render(const Transform &transform) = 0;
};


#endif
