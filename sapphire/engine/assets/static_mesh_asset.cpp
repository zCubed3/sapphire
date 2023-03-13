#include "static_mesh_asset.hpp"

#include <cstring>

#include <engine/rendering/buffers/mesh_buffer.hpp>
#include <engine/rendering/render_server.hpp>

void StaticMeshAsset::create_primitives() {
    /*
    if (primitive_quad == nullptr) {
        primitive_quad = new StaticMeshAsset();

        uint32_t quad_triangles[] = {
                2, 1, 0, 1, 2, 3
        };

        glm::vec3 quad_positions[] = {
                {-1, -1, 0},
                {-1, 1, 0},
                {1, -1, 0},
                {1, 1, 0},
        };

        glm::vec3 quad_normals[] = {
                {0, 0, 1},
                {0, 0, 1},
                {0, 0, 1},
                {0, 0, 1},
        };

        glm::vec2 quad_uvs[] = {
                {0, 0},
                {0, 1},
                {1, 0},
                {1, 1},
        };

        primitive_quad->set_triangle_data(quad_triangles, 6);
        primitive_quad->set_position_data(quad_positions, 4);
        primitive_quad->set_normal_data(quad_normals, 4);
        primitive_quad->set_uv0_data(quad_uvs, 4);

        RenderServer::get_singleton()->populate_mesh_buffer(primitive_quad);
    }

    if (primitive_cube == nullptr) {
        primitive_cube = new StaticMeshAsset();

        uint32_t triangles[] = {
                2, 1, 0, 1, 2, 3
        };

        glm::vec3 positions[] = {
                {-1, -1, 0},
                {-1, 1, 0},
                {1, -1, 0},
                {1, 1, 0},
        };

        glm::vec3 normals[] = {
                {0, 0, 1},
                {0, 0, 1},
                {0, 0, 1},
                {0, 0, 1},
        };

        glm::vec2 uvs[] = {
                {0, 0},
                {0, 1},
                {1, 0},
                {1, 1},
        };

        primitive_cube->set_triangle_data(triangles, 6);
        primitive_cube->set_position_data(positions, 4);
        primitive_cube->set_normal_data(normals, 4);
        primitive_cube->set_uv0_data(uvs, 4);

        RenderServer::get_singleton()->populate_mesh_buffer(primitive_cube);
    }
    */
}

std::shared_ptr<StaticMeshAsset> StaticMeshAsset::missing_model = nullptr;
StaticMeshAsset *StaticMeshAsset::primitive_quad = nullptr;
StaticMeshAsset *StaticMeshAsset::primitive_cube = nullptr;

StaticMeshAsset::StaticMeshAsset() = default;

StaticMeshAsset::StaticMeshAsset(const std::vector<Vertex> &vertex_data, const std::vector<index_t> &index_data) {
    this->vertex_data = vertex_data;
    this->index_data = index_data;
}

void StaticMeshAsset::draw_editor_gui() {

}

void StaticMeshAsset::set_vertex_data(const std::vector<Vertex> &vertex_data) {
    this->vertex_data = vertex_data;
}

void StaticMeshAsset::set_index_data(const std::vector<index_t> &index_data) {
    this->index_data = index_data;
}

MeshAsset::Vertex *StaticMeshAsset::get_vertex_data(uint32_t *p_length) {
    if (p_length != nullptr) {
        *p_length = vertex_data.size();
    }

    return vertex_data.data();
}

MeshAsset::index_t *StaticMeshAsset::get_index_data(uint32_t *p_length) {
    if (p_length != nullptr) {
        *p_length = index_data.size();
    }

    return index_data.data();
}

uint32_t StaticMeshAsset::get_vertex_count() {
    return static_cast<uint32_t>(vertex_data.size());
}

uint32_t StaticMeshAsset::get_index_count() {
    return static_cast<uint32_t>(index_data.size());
}

void StaticMeshAsset::draw(ObjectBuffer *p_object_buffer, Material *p_material) {
    if (buffer != nullptr) {
        //buffer->draw(p_object_buffer, p_material);
    }
}

StaticMeshAsset *StaticMeshAsset::get_primitive(Primitive primitive) {
    switch (primitive) {
        default:
            return nullptr;

        case Primitive::PRIMITIVE_QUAD:
            return primitive_quad;
    }
}