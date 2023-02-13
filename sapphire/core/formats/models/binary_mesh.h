#ifndef SAPPHIRE_SEBM_H
#define SAPPHIRE_SEBM_H

#include <cstdint>
#include <string>
#include <vector>

#include <core/data/vectors.h>

// Default SEBM identifier
const uint32_t SEBM_IDENT = 0x4D424553;

// Sapphire Engine Binary Mesh (aka sebm)
// Pre-welded and compact mesh format
// Used internally for engine resources
class BinaryMesh {
public:
    typedef uint32_t index_t;

    struct Header {
        uint32_t ident = SEBM_IDENT;
        uint32_t vertices_count;
        uint32_t indexes_count;
    };

    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector2 uv0;
    };

    std::vector<Vertex> vertices;
    std::vector<index_t> indexes;

    static BinaryMesh *read_from_path(const std::string& path);

    bool write_to_path(const std::string& path);
};

#endif
