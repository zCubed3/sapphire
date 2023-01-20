#ifndef AGE_MESH_ASSET_H
#define AGE_MESH_ASSET_H

#include <engine/assets/asset.h>

#include <glm.hpp>

class MeshAsset : public Asset {
public:
    virtual glm::vec3 *get_position_data(size_t *p_length) = 0;
    virtual glm::vec3 *get_normal_data(size_t *p_length) = 0;
    virtual glm::vec2 *get_uv0_data(size_t *p_length) = 0;
    virtual glm::vec4 *get_tangent_data(size_t *p_length) = 0;
    virtual uint32_t *get_triangle_data(size_t *p_length) = 0;
};


#endif
