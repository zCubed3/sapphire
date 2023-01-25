#ifndef SAPPHIRE_MESH_BUFFER_H
#define SAPPHIRE_MESH_BUFFER_H

// TODO: Come up with a better name than "MeshBuffer"?

#include <glm.hpp>

#include <engine/data/transform.h>

class ShaderAsset;

// Abstract render server mesh info
class MeshBuffer {
public:
    // TODO: Make creating meshes faster by allowing meshes to generate their own vertex data
    struct Vertex {
        glm::vec3 position = {};
        glm::vec3 normal = {};
        glm::vec2 uv0 = {};
    };

    virtual void render(const Transform &transform, ShaderAsset *p_shader_asset);
    virtual void render(const glm::mat4 &matrix, ShaderAsset *p_shader_asset);
    virtual void render(ShaderAsset *p_shader_asset);

    virtual ~MeshBuffer() = default;
};

#endif
