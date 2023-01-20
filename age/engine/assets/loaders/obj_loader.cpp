#include "obj_loader.h"

#include <fstream>

#include <engine/assets/static_mesh_asset.h>

struct OBJIndice {
    uint32_t v = 0;
    uint32_t t = 0;
    uint32_t n = 0;
};

std::vector<std::string> OBJLoader::get_extensions() {
    return {"obj"};
}

Asset *OBJLoader::load_from_path(const std::string &path) {
    std::ifstream file(path);

    if (file.is_open()) {
        std::vector<glm::vec3> unweld_positions;
        std::vector<glm::vec3> unweld_normals;
        std::vector<glm::vec2> unweld_tex_coords;
        std::vector<OBJIndice> unweld_triangles;

        std::string line;
        while (std::getline(file, line)) {
            // First two characters are commonly a data id
            std::string id = line.substr(0, 2);

            // Comment
            if (id[0] == '#') {
                continue;
            }

            std::string contents = line.substr(2);

            // TODO: Named meshes?
            /*
            if (id[0] == 'o') {
                obj->name = contents;
            }
            */

            // Vertex data
            if (id[0] == 'v') {

                // Texcoord
                if (id[1] == 't') {
                    // TODO: Replace sscanf?
                    glm::vec2 uv {};

                    sscanf(contents.c_str(), "%f %f", &uv.x, &uv.y);
                    unweld_tex_coords.emplace_back(uv);

                    continue;
                }

                glm::vec3 v3_data {};
                sscanf(contents.c_str(), "%f %f %f", &v3_data.x, &v3_data.y, &v3_data.z);

                if (id[1] == 'n')
                    unweld_normals.emplace_back(v3_data);
                else
                    unweld_positions.emplace_back(v3_data);
            }

            // F = face, we either iterate 3 times to make a tri, or once to make it a vert, depends on the obj
            // TODO: Make this less ugly?
            if (id[0] == 'f') {
                std::string face = contents;

                size_t pos;
                while (true) {
                    OBJIndice tri {};
                    pos = face.find(' ');

                    sscanf(face.c_str(), "%i/%i/%i",
                           &tri.v, &tri.t, &tri.n
                    );

                    face.erase(0, pos + 1);

                    tri.v -= 1;
                    tri.t -= 1;
                    tri.n -= 1;

                    unweld_triangles.emplace_back(tri);

                    if (pos == std::string::npos)
                        break;
                }
            }
        }

        // We then need to weld the triangles
        std::vector<glm::vec3> weld_positions;
        std::vector<glm::vec3> weld_normals;
        std::vector<glm::vec2> weld_tex_coords;
        std::vector<uint32_t> weld_indices;
        uint32_t welded = 0;

        for (size_t t = 0; t < unweld_triangles.size(); t++) {
            glm::vec3 position = unweld_positions[unweld_triangles[t].v];
            glm::vec2 tex_coord = unweld_tex_coords[unweld_triangles[t].t];
            glm::vec3 normal = unweld_normals[unweld_triangles[t].n];

            // Check that no existing vertex matches this one (this becomes exponentially slower!)
            bool matching = false;
            for (size_t o = 0; o < welded; o++) {
                glm::vec3 test_position = weld_positions[weld_indices[o]];
                glm::vec2 test_tex_coord = weld_tex_coords[weld_indices[o]];
                glm::vec3 test_normal = weld_normals[weld_indices[o]];

                if (position == test_position && tex_coord == test_tex_coord && normal == test_normal) {
                    weld_indices.emplace_back((uint32_t)o);
                    matching = true;
                    break;
                }
            }

            if (!matching) {
                weld_positions.emplace_back(position);
                weld_tex_coords.emplace_back(tex_coord);
                weld_normals.emplace_back(normal);
                weld_indices.emplace_back(welded);

                welded += 1;
            }
        }

        auto mesh = new StaticMeshAsset();
        
        mesh->set_position_data(weld_positions.data(), welded);
        mesh->set_normal_data(weld_normals.data(), welded);
        mesh->set_uv0_data(weld_tex_coords.data(), welded);

        mesh->set_triangle_data(weld_indices.data(), weld_indices.size());

        return mesh;
    }

    return nullptr;
}