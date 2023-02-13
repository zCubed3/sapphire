#ifndef SAPPHIRE_MESH_BUFFER_H
#define SAPPHIRE_MESH_BUFFER_H

// TODO: Come up with a better name than "MeshBuffer"?

#include <glm.hpp>

#include <engine/scene/transform.h>

#include <memory>

class Material;
class ObjectBuffer;

// Abstract render server mesh info
class MeshBuffer {
public:
    virtual void draw(ObjectBuffer* p_object_buffer, std::shared_ptr<Material> p_material) = 0;

    virtual ~MeshBuffer() = default;
};

#endif
