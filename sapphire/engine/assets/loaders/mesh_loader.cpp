#include "mesh_loader.h"

#include <fstream>

#include <core/formats/models/binary_mesh.h>
#include <core/formats/models/wavefront_obj.h>
#include <core/data/string_tools.h>

#include <engine/assets/static_mesh_asset.h>
#include <engine/rendering/render_server.h>

void MeshLoader::load_placeholders() {
    StaticMeshAsset::create_primitives();
}

std::vector<std::string> MeshLoader::get_extensions() {
    return {"obj", "sebm"};
}

std::shared_ptr<Asset> MeshLoader::load_from_path(const std::string &path, const std::string& extension) {
    StaticMeshAsset *mesh = nullptr;

    // TODO: CACHE CACHE CACHE!
    if (StringTools::compare(extension, "obj")) {
        WavefrontOBJ *obj = WavefrontOBJ::read_obj_from_path(path);

        if (obj != nullptr) {
            // TODO: MERGE SUBMESHES
            size_t vertex_count = obj->objects[0].vertices.size();
            size_t index_count = obj->objects[0].indexes.size();

            MeshAsset::Vertex* vertices = new MeshAsset::Vertex[vertex_count];
            MeshAsset::index_t* indexes = new MeshAsset::index_t[index_count];

            for (int v = 0; v < obj->objects[0].vertices.size(); v++) {
                WavefrontOBJ::Vertex& obj_vertex = obj->objects[0].vertices[v];

                vertices[v] = {
                        {obj_vertex.position.x, obj_vertex.position.y, obj_vertex.position.z},
                        {obj_vertex.normal.x, obj_vertex.normal.y, obj_vertex.normal.z},
                        {obj_vertex.uv0.x, obj_vertex.uv0.y},
                };
            }

            for (int i = 0; i < obj->objects[0].indexes.size(); i++) {
                indexes[i] = obj->objects[0].indexes[i];
            }

            mesh = new StaticMeshAsset(vertices, vertex_count, indexes, index_count);
            delete obj;
        }
    }

    if (StringTools::compare(extension, "sebm")) {
        BinaryMesh *binary_mesh = BinaryMesh::read_from_path(path);

        if (binary_mesh != nullptr) {
            // TODO: MERGE SUBMESHES
            size_t vertex_count = binary_mesh->vertices.size();
            size_t index_count = binary_mesh->indexes.size();

            MeshAsset::Vertex* vertices = new MeshAsset::Vertex[vertex_count];
            MeshAsset::index_t* indexes = new MeshAsset::index_t[index_count];

            for (int v = 0; v < binary_mesh->vertices.size(); v++) {
                BinaryMesh::Vertex& bin_vertex = binary_mesh->vertices[v];

                vertices[v] = {
                        {bin_vertex.position.x, bin_vertex.position.y, bin_vertex.position.z},
                        {bin_vertex.normal.x, bin_vertex.normal.y, bin_vertex.normal.z},
                        {bin_vertex.uv0.x, bin_vertex.uv0.y},
                };
            }

            for (int i = 0; i < binary_mesh->indexes.size(); i++) {
                indexes[i] = binary_mesh->indexes[i];
            }

            mesh = new StaticMeshAsset(vertices, vertex_count, indexes, index_count);
            delete binary_mesh;
        }
    }

    if (mesh != nullptr) {
        RenderServer::get_singleton()->populate_mesh_buffer(mesh);
        return cache_asset(path, mesh);
    }

    return nullptr;
}