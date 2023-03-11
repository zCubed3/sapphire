#ifndef SAPPHIRE_STATIC_MESH_ASSET_H
#define SAPPHIRE_STATIC_MESH_ASSET_H

#include <engine/assets/mesh_asset.h>

#include <vector>

// Mesh that is loaded from a file at runtime
class StaticMeshAsset : public MeshAsset {
protected:
    friend class MeshLoader;

    static void create_primitives();

    static StaticMeshAsset *primitive_quad;
    static StaticMeshAsset *primitive_cube;

    std::vector<MeshAsset::Vertex> vertex_data;
    std::vector<MeshAsset::index_t> index_data;

public:
    static std::shared_ptr<StaticMeshAsset> missing_model;

    enum Primitive {
        PRIMITIVE_QUAD,
        PRIMITIVE_CUBE
    };

    StaticMeshAsset();
    StaticMeshAsset(const std::vector<Vertex>& vertex_data, const std::vector<index_t>& index_data);

    void draw_editor_gui() override;

    void set_vertex_data(const std::vector<Vertex>& vertex_data);
    void set_index_data(const std::vector<index_t>& index_data);

    Vertex *get_vertex_data(uint32_t *p_length) override;
    index_t *get_index_data(uint32_t *p_length) override;

    uint32_t get_vertex_count() override;
    uint32_t get_index_count() override;

    void draw(ObjectBuffer *p_object_buffer, Material *p_material) override;

    // TODO: ProceduralMeshAsset for procedural primitives?
    static StaticMeshAsset *get_primitive(Primitive primitive);
};

#endif
