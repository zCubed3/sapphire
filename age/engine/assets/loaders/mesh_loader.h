#ifndef AGE_MESH_LOADER_H
#define AGE_MESH_LOADER_H

#include <glm.hpp>

#include <vector>

#include <engine/assets/asset_loader.h>

class MeshLoader : public AssetLoader {
public:
    std::vector<std::string> get_extensions() override;

    Asset *load_from_path(const std::string &path) override;
};

#endif
