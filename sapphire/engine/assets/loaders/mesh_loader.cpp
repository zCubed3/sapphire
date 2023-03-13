#include "mesh_loader.hpp"

#include <iostream>
#include <fstream>

#include <core/data/string_tools.hpp>
#include <core/formats/models/binary_mesh.h>
#include <core/formats/models/wavefront_obj.h>

#include <engine/assets/static_mesh_asset.hpp>
#include <engine/rendering/render_server.hpp>

void MeshLoader::load_placeholders() {
    StaticMeshAsset::create_primitives();

    // TODO: Bake this into the engine
    StaticMeshAsset::missing_model = std::reinterpret_pointer_cast<StaticMeshAsset>(load_from_path("../LoadingPlaceholder.sebm", "sebm"));
}

std::vector<std::string> MeshLoader::get_extensions() {
    return {"obj", "sebm"};
}

// TODO: Transactional loading?
std::shared_ptr<Asset> MeshLoader::load_from_path(const std::string &path, const std::string& extension) {
    StaticMeshAsset *mesh = new StaticMeshAsset();

    // We can return the asset ahead of time
    // But begin loading it now!
    loader_pool.submit_func([=](){
        std::vector<MeshAsset::Vertex> vertices;
        std::vector<MeshAsset::index_t> indexes;

        // TODO: CACHE CACHE CACHE!
        if (StringTools::compare(extension, "obj")) {
            WavefrontOBJ *obj = WavefrontOBJ::read_obj_from_path(path);

            if (obj != nullptr) {
                // TODO: MERGE SUBMESHES
                size_t vertex_count = obj->objects[0].vertices.size();
                size_t index_count = obj->objects[0].indexes.size();

                for (WavefrontOBJ::Vertex& obj_vertex: obj->objects[0].vertices) {
                    MeshAsset::Vertex vertex = {
                            {obj_vertex.position.x, obj_vertex.position.y, obj_vertex.position.z},
                            {obj_vertex.normal.x, obj_vertex.normal.y, obj_vertex.normal.z},
                            {obj_vertex.uv0.x, obj_vertex.uv0.y},
                    };

                    vertices.push_back(vertex);
                }

                for (int i = 0; i < obj->objects[0].indexes.size(); i++) {
                    indexes.push_back(obj->objects[0].indexes[i]);
                }

                //mesh = new StaticMeshAsset(vertices, indexes);
                mesh->set_vertex_data(vertices);
                mesh->set_index_data(indexes);

                delete obj;
            }
        }

        if (StringTools::compare(extension, "sebm")) {
            BinaryMesh *binary_mesh = BinaryMesh::read_from_path(path);

            if (binary_mesh != nullptr) {
                // TODO: MERGE SUBMESHES
                size_t vertex_count = binary_mesh->vertices.size();
                size_t index_count = binary_mesh->indexes.size();

                for (BinaryMesh::Vertex& bin_vertex : binary_mesh->vertices) {
                    MeshAsset::Vertex vertex = {
                            {bin_vertex.position.x, bin_vertex.position.y, bin_vertex.position.z},
                            {bin_vertex.normal.x, bin_vertex.normal.y, bin_vertex.normal.z},
                            {bin_vertex.uv0.x, bin_vertex.uv0.y},
                    };

                    vertices.push_back(vertex);
                }

                for (int i = 0; i < binary_mesh->indexes.size(); i++) {
                    indexes.push_back(binary_mesh->indexes[i]);
                }

                //mesh = new StaticMeshAsset(vertices, indexes);
                mesh->set_vertex_data(vertices);
                mesh->set_index_data(indexes);

                delete binary_mesh;
            }
        }

        std::cout << "Loaded: " << path << std::endl;
        RenderServer::get_singleton()->populate_mesh_buffer(mesh);
    });

    return cache_asset(path, mesh);
}