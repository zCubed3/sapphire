#include "binary_mesh.h"

#include <fstream>

BinaryMesh *BinaryMesh::read_from_path(const std::string &path) {
    std::ifstream file(path, std::ios::binary);

    if (file.is_open()) {
        Header header {};
        file.read((char*)&header, sizeof(Header));

        if (header.ident != SEBM_IDENT) {
            return nullptr;
        }

        BinaryMesh *mesh = new BinaryMesh();
        mesh->vertices.resize(header.vertices_count);
        mesh->indexes.resize(header.indexes_count);

        file.read((char*)mesh->vertices.data(), sizeof(Vertex) * header.vertices_count);
        file.read((char*)mesh->indexes.data(), sizeof(index_t) * header.indexes_count);

        return mesh;
    }

    return nullptr;
}

bool BinaryMesh::write_to_path(const std::string &path) {
    std::ofstream file(path, std::ios::binary);

    if (file.is_open()) {
        Header header {};
        header.ident = SEBM_IDENT;
        header.vertices_count = vertices.size();
        header.indexes_count = indexes.size();

        file.write((char*)&header, sizeof(Header));
        file.write((char*)vertices.data(), sizeof(Vertex) * vertices.size());
        file.write((char*)indexes.data(), sizeof(index_t) * indexes.size());

        return true;
    }

    return false;
}