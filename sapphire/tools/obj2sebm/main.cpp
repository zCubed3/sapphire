#include <iostream>

#include <core/formats/models/binary_mesh.h>
#include <core/formats/models/wavefront_obj.h>

// Converts a Wavefront OBJ to a Sapphire Engine Binary Mesh
int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Proper usage of OBJ2SEM is as follows:\n";
        std::cout << "./obj2sebm [obj path] [sebm path]" << std::endl;

        return 0;
    } else {
        WavefrontOBJ *obj = WavefrontOBJ::read_obj_from_path(argv[1]);

        BinaryMesh *binary_mesh = new BinaryMesh();

        // TODO: Merge submeshes?
        for (WavefrontOBJ::Vertex& vertex: obj->objects[0].vertices) {
            BinaryMesh::Vertex binary_vertex {
                vertex.position,
                vertex.normal,
                vertex.uv0
            };

            binary_mesh->vertices.push_back(binary_vertex);
        }

        for (WavefrontOBJ::index_t& index: obj->objects[0].indexes) {
            binary_mesh->indexes.push_back(index);
        }

        binary_mesh->write_to_path(argv[2]);
    }
}