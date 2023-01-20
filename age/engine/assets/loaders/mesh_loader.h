#ifndef AGE_MESH_LOADER_H
#define AGE_MESH_LOADER_H

#include <glm.hpp>

#include <vector>

#include <engine/assets/asset_loader.h>

class MeshLoader : public AssetLoader {
public:
    Asset *load_from_path(const std::string &path) override;
};

#endif
