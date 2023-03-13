#ifndef SAPPHIRE_WAVEFRONT_OBJ_H
#define SAPPHIRE_WAVEFRONT_OBJ_H

#include <string>
#include <vector>

#include <core/data/vectors.hpp>

// Wavefront obj (aka .obj file)
class WavefrontOBJ {
public:
    // TODO: Unwelded OBJs?
    typedef uint32_t index_t;

    struct Triangle {
        uint32_t v;
        uint32_t t;
        uint32_t n;
    };

public:
    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector2 uv0;
    };

    struct Object {
        std::string name;
        std::vector<Vertex> vertices;
        std::vector<index_t> indexes;
    };

    struct UnweldedData {
        std::vector<Vector3> positions;
        std::vector<Vector3> normals;
        std::vector<Vector2> uv0;
        std::vector<Triangle> triangles;

        void clear();
    };

    std::vector<Object> objects;

    static void weld_data(Object& obj, const UnweldedData& data, bool optimize = true);

    static WavefrontOBJ* read_obj_from_path(const std::string& path);
};

#endif
