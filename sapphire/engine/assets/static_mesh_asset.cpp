#include "static_mesh_asset.h"

#include <engine/rendering/mesh_buffer.h>

void StaticMeshAsset::set_position_data(glm::vec3 *data, size_t length) {
    delete[] position_data;

    position_data = new glm::vec3[length];
    memcpy(position_data, data, sizeof(glm::vec3) * length);

    // TODO: Validate vertex count and data sizes!
    vertex_count = length;
}

void StaticMeshAsset::set_normal_data(glm::vec3 *data, size_t length) {
    delete[] normal_data;

    normal_data = new glm::vec3[length];
    memcpy(normal_data, data, sizeof(glm::vec3) * length);

    // TODO: Validate vertex count and data sizes!
    vertex_count = length;
}

void StaticMeshAsset::set_uv0_data(glm::vec2 *data, size_t length) {
    delete[] uv0_data;

    uv0_data = new glm::vec2[length];
    memcpy(uv0_data, data, sizeof(glm::vec2) * length);

    // TODO: Validate vertex count and data sizes!
    vertex_count = length;
}

void StaticMeshAsset::set_tangent_data(glm::vec4 *data, size_t length) {
    delete[] tangent_data;

    tangent_data = new glm::vec4[length];
    memcpy(tangent_data, data, sizeof(glm::vec4) * length);

    // TODO: Validate vertex count and data sizes!
    vertex_count = length;
}

void StaticMeshAsset::set_triangle_data(uint32_t *data, size_t length) {
    delete[] triangle_data;

    triangle_data = new uint32_t[length];
    memcpy(triangle_data, data, sizeof(uint32_t) * length);

    // TODO: Validate vertex count and data sizes!
    triangle_count = length;
}

glm::vec3 *StaticMeshAsset::get_position_data(size_t *p_length) {
    if (p_length != nullptr) {
        *p_length = vertex_count;
    }

    return position_data;
}

glm::vec3 *StaticMeshAsset::get_normal_data(size_t *p_length) {
    if (p_length != nullptr) {
        *p_length = vertex_count;
    }

    return normal_data;
}

glm::vec2 *StaticMeshAsset::get_uv0_data(size_t *p_length) {
    if (p_length != nullptr) {
        *p_length = vertex_count;
    }

    return uv0_data;
}

glm::vec4 *StaticMeshAsset::get_tangent_data(size_t *p_length) {
    if (p_length != nullptr) {
        *p_length = vertex_count;
    }

    return tangent_data;
}

uint32_t *StaticMeshAsset::get_triangle_data(size_t *p_length) {
    if (p_length != nullptr) {
        *p_length = triangle_count;
    }

    return triangle_data;
}

uint32_t StaticMeshAsset::get_vertex_count() {
    return vertex_count;
}

uint32_t StaticMeshAsset::get_triangle_count() {
    return triangle_count;
}

void StaticMeshAsset::render(const Transform &transform) {
    if (buffer != nullptr) {
        buffer->render(transform, shader);
    }
}
