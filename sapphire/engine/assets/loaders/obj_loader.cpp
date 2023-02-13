#include "obj_loader.h"

#include <fstream>

#include <core/formats/models/wavefront_obj.h>
#include <core/data/string_tools.h>

#include <engine/assets/static_mesh_asset.h>
#include <engine/rendering/render_server.h>

void OBJLoader::load_placeholders() {
    StaticMeshAsset::create_primitives();
}

std::vector<std::string> OBJLoader::get_extensions() {
    return {"obj"};
}

std::shared_ptr<Asset> OBJLoader::load_from_path(const std::string &path, const std::string& extension) {
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
        }
    }

    if (mesh != nullptr) {
        RenderServer::get_singleton()->populate_mesh_buffer(mesh);
        return cache_asset(path, mesh);
    }

    return nullptr;
}