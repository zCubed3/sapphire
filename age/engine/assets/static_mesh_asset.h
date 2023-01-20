#ifndef AGE_STATIC_MESH_ASSET_H
#define AGE_STATIC_MESH_ASSET_H

#include <engine/assets/mesh_asset.h>

// Mesh that can't be modified at runtime
// It is loaded from a file then is immutable
class StaticMeshAsset : public MeshAsset {
protected:
    glm::vec3 *get_position_data(size_t *p_length) override;
    glm::vec3 *get_normal_data(size_t *p_length) override;
    glm::vec2 *get_uv0_data(size_t *p_length) override;
    glm::vec4 *get_tangent_data(size_t *p_length) override;
    uint32_t * get_triangle_data(size_t *p_length) override;
};

#endif
