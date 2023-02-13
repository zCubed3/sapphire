#include "wavefront_obj.h"

#include <fstream>

void WavefrontOBJ::UnweldedData::clear() {
    positions.clear();
    normals.clear();
    uv0.clear();
    triangles.clear();
}

void WavefrontOBJ::weld_data(Object &obj, const UnweldedData &data, bool optimize) {
    uint32_t welded = 0;

    if (optimize) {
        for (size_t t = 0; t < data.triangles.size(); t++) {
            const Triangle& triangle = data.triangles[t];

            const Vector3& position = data.positions[triangle.v];
            const Vector2& uv0 = data.uv0[triangle.t];
            const Vector3& normal = data.normals[triangle.n];

            // Check that no existing vertex matches this one (this becomes exponentially slower!)
            bool matching = false;
            for (size_t o = 0; o < welded; o++) {
                index_t index = obj.indexes[o];

                const Vertex& vertex = obj.vertices[o];

                bool position_pass = vertex.position == position;
                bool normal_pass = vertex.normal == normal;
                bool uv0_pass = vertex.uv0 == uv0;

                if (position_pass && normal_pass && uv0_pass) {
                    obj.indexes.emplace_back(static_cast<uint32_t>(o));
                    matching = true;
                    break;
                }
            }

            if (!matching) {
                obj.indexes.emplace_back(welded);

                welded += 1;

                obj.vertices.push_back({position, normal, uv0});
            }
        }
    } else {
        for (size_t t = 0; t < data.triangles.size(); t++) {
            const Triangle& triangle = data.triangles[t];

            const Vector3& position = data.positions[triangle.v];
            const Vector2& uv0 = data.uv0[triangle.t];
            const Vector3& normal = data.normals[triangle.n];

            obj.vertices.push_back({position, normal, uv0});
            obj.indexes.emplace_back(welded);
            welded += 1;
        }
    }
}

WavefrontOBJ *WavefrontOBJ::read_obj_from_path(const std::string &path) {
    std::ifstream file(path);

    if (file.is_open()) {
        WavefrontOBJ* obj = new WavefrontOBJ();

        UnweldedData data;
        Object object;
        object.name = "";

        bool empty = true;
        std::string line;
        while (std::getline(file, line)) {
            // First two characters are commonly a data id
            std::string id = line.substr(0, 2);

            // Comment
            if (id[0] == '#') {
                continue;
            }

            std::string contents = line.substr(2);

            if (id[0] == 'o') {
                if (!object.name.empty()) {
                    weld_data(object, data);
                    obj->objects.push_back(object);

                    object = Object();
                    data.clear();
                    empty = true;
                }

                object.name = contents;
            }

            // Vertex data
            if (id[0] == 'v') {
                empty = false;

                // Texcoord
                if (id[1] == 't') {
                    // TODO: Replace sscanf?
                    Vector2 uv{};

                    sscanf(contents.c_str(), "%f %f", &uv.x, &uv.y);

                    // OBJ UVs are upside down
                    uv.y = 1.0F - uv.y;

                    data.uv0.emplace_back(uv);

                    continue;
                }

                Vector3 v3_data{};
                sscanf(contents.c_str(), "%f %f %f", &v3_data.x, &v3_data.y, &v3_data.z);

                if (id[1] == 'n')
                    data.normals.emplace_back(v3_data);
                else
                    data.positions.emplace_back(v3_data);
            }

            // F = face, we either iterate 3 times to make a tri, or once to make it a vert, depends on the obj
            // TODO: Make this less ugly?
            if (id[0] == 'f') {
                std::string face = contents;

                size_t pos;
                while (true) {
                    Triangle tri{};
                    pos = face.find(' ');

                    sscanf(face.c_str(), "%i/%i/%i", &tri.v, &tri.t, &tri.n);

                    face.erase(0, pos + 1);

                    tri.v -= 1;
                    tri.t -= 1;
                    tri.n -= 1;

                    data.triangles.emplace_back(tri);

                    if (pos == std::string::npos)
                        break;
                }
            }
        }

        if (!empty) {
            weld_data(object, data);
            obj->objects.push_back(object);
        }

        return obj;
    }

    return nullptr;
}