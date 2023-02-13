#ifndef SAPPHIRE_MESH_ASSET_H
#define SAPPHIRE_MESH_ASSET_H

#include <engine/assets/asset.h>
#include <engine/scene/transform.h>

#include <memory>

#include <glm.hpp>

class Material;
class MeshBuffer;
class ObjectBuffer;

class MeshAsset : public Asset {
public:
    typedef uint32_t index_t;

    // TODO: Allow additional vertex data?
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv0;
    };

    // TODO: Asset safety so dangling pointers can't happen
    std::shared_ptr<MeshBuffer> buffer = nullptr;

    ~MeshAsset() override;

    virtual Vertex *get_vertex_data(uint32_t *p_length) = 0;
    virtual index_t *get_index_data(uint32_t *p_length) = 0;

    virtual uint32_t get_vertex_count() = 0;
    virtual uint32_t get_index_count() = 0;

    virtual void draw(ObjectBuffer *p_object_buffer, Material *p_material) = 0;
};


#endif
