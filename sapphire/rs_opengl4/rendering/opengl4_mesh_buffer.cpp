#include "opengl4_mesh_buffer.h"

#include <gtc/matrix_transform.hpp>

#include <engine/assets/mesh_asset.h>
#include <engine/rendering/render_server.h>
#include <engine/rendering/render_target.h>
#include <engine/scene/world.h>
#include <glad/glad.h>
#include <rs_opengl4/assets/glsl_shader_asset.h>

// TODO: Make creating meshes faster by allowing meshes to generate their own interleaved data
struct InterleavedVertexData {
    glm::vec3 position = {};
    glm::vec3 normal = {};
    glm::vec2 uv0 = {};
    glm::vec4 tangent = {};
};

OpenGL4MeshBuffer::OpenGL4MeshBuffer(MeshAsset *p_mesh_asset) {
    // TODO: Allow creating mesh buffers without mesh assets?
    // TODO: Other types of mesh buffer for instancing, skinning, etc?
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    // TODO: Updating buffers post-construction
    // TODO: Optional channels?
    InterleavedVertexData *vertices = new InterleavedVertexData[p_mesh_asset->get_vertex_count()];

    uint32_t *triangles = p_mesh_asset->get_triangle_data(nullptr);
    glm::vec3 *positions = p_mesh_asset->get_position_data(nullptr);
    glm::vec3 *normals = p_mesh_asset->get_normal_data(nullptr);
    glm::vec2 *tex_coords = p_mesh_asset->get_uv0_data(nullptr);
    glm::vec4 *tangents = p_mesh_asset->get_tangent_data(nullptr);

    tri_count = p_mesh_asset->get_triangle_count();

    for (int v = 0; v < p_mesh_asset->get_vertex_count(); v++) {
        if (positions != nullptr) {
            vertices[v].position = positions[v];
        }

        if (normals != nullptr) {
            vertices[v].normal = normals[v];
        }

        if (tex_coords != nullptr) {
            vertices[v].uv0 = tex_coords[v];
        }

        if (tangents != nullptr) {
            vertices[v].tangent = tangents[v];
        }
    }

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(InterleavedVertexData) * p_mesh_asset->get_vertex_count(), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * p_mesh_asset->get_triangle_count(), triangles, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGL4MeshBuffer::render(const Transform &transform, ShaderAsset *p_shader_asset) {
    GLSLShaderAsset *glsl_shader = static_cast<GLSLShaderAsset *>(p_shader_asset);

    // TODO: ShaderAsset contains get_placeholder() instead?
    if (glsl_shader == nullptr) {
        glsl_shader = GLSLShaderAsset::get_placeholder();
    }

    MeshBuffer::render(transform, p_shader_asset);

    glUseProgram(glsl_shader->shader_handle);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    const size_t vertex_size = sizeof(InterleavedVertexData);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertex_size, (void *) (sizeof(float) * 3));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertex_size, (void *) (sizeof(float) * 6));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, vertex_size, (void *) (sizeof(float) * 8));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawElements(GL_TRIANGLES, tri_count, GL_UNSIGNED_INT, nullptr);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);

    glBindVertexArray(0);
}