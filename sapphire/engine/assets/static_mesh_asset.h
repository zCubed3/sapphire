#ifndef SAPPHIRE_STATIC_MESH_ASSET_H
#define SAPPHIRE_STATIC_MESH_ASSET_H

#include <engine/assets/mesh_asset.h>

#include <vector>

// Mesh that is loaded from a file at runtime
class StaticMeshAsset : public MeshAsset {
protected:
    friend class OBJLoader;

    static void create_primitives();

    static StaticMeshAsset *primitive_quad;
    static StaticMeshAsset *primitive_cube;

    MeshAsset::Vertex *vertex_data = nullptr;
    MeshAsset::index_t *index_data = nullptr;

    size_t vertex_count = 0;
    size_t index_count = 0;

public:
    enum Primitive {
        PRIMITIVE_QUAD,
        PRIMITIVE_CUBE
    };

    StaticMeshAsset(Vertex* vertex_data, size_t vertex_count, index_t* index_data, size_t index_count);

    Vertex *get_vertex_data(uint32_t *p_length) override;
    index_t *get_index_data(uint32_t *p_length) override;

    uint32_t get_vertex_count() override;
    uint32_t get_index_count() override;

    void draw(ObjectBuffer *p_object_buffer, Material *p_material) override;

    // TODO: ProceduralMeshAsset for procedural primitives?
    static StaticMeshAsset *get_primitive(Primitive primitive);
};

#endif
