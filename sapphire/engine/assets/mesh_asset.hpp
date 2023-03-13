#ifndef SAPPHIRE_MESH_ASSET_HPP
#define SAPPHIRE_MESH_ASSET_HPP

#include <engine/assets/asset.hpp>
#include <engine/scene/transform.hpp>

#include <memory>

#include <glm.hpp>

class Material;
class MeshBuffer;
class ObjectBuffer;

class MeshAsset : public Asset {
    REFLECT_CLASS(MeshAsset, Asset)

public:
    typedef uint32_t index_t;

    // TODO: Allow additional vertex data?
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv0;
    };

    std::shared_ptr<MeshBuffer> buffer = nullptr;

    ~MeshAsset() override;

#if defined(SAPPHIRE_EDITOR)
    void draw_editor_gui() override;
#endif

    virtual Vertex *get_vertex_data(uint32_t *p_length) = 0;
    virtual index_t *get_index_data(uint32_t *p_length) = 0;

    virtual uint32_t get_vertex_count() = 0;
    virtual uint32_t get_index_count() = 0;

    virtual void draw(ObjectBuffer *p_object_buffer, Material *p_material) = 0;
};


#endif
