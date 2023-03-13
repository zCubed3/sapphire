#ifndef SAPPHIRE_MESH_LOADER_HPP
#define SAPPHIRE_MESH_LOADER_HPP

#include <glm.hpp>

#include <vector>

#include <engine/assets/asset_loader.hpp>

class MeshLoader : public AssetLoader {
    REFLECT_CLASS(MeshLoader, AssetLoader)

protected:
    void load_placeholders() override;

public:
    std::vector<std::string> get_extensions() override;

    std::shared_ptr<Asset> load_from_path(const std::string &path, const std::string& extension) override;
};

#endif
