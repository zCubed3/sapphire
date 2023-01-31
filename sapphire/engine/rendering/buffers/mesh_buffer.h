#ifndef SAPPHIRE_MESH_BUFFER_H
#define SAPPHIRE_MESH_BUFFER_H

// TODO: Come up with a better name than "MeshBuffer"?

#include <glm.hpp>

#include <engine/scene/transform.h>

class Material;
class ObjectBuffer;

// Abstract render server mesh info
class MeshBuffer {
public:
    // TODO: Make creating meshes faster by allowing meshes to generate their own vertex data
    struct Vertex {
        glm::vec3 position = {};
        glm::vec3 normal = {};
        glm::vec2 uv0 = {};
    };

    virtual void render(ObjectBuffer* p_object_buffer, Material *p_material) = 0;

    virtual ~MeshBuffer() = default;
};

#endif
