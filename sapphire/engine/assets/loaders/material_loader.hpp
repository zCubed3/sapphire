#ifndef SAPPHIRE_MATERIAL_LOADER_HPP
#define SAPPHIRE_MATERIAL_LOADER_HPP

#include <engine/assets/asset_loader.hpp>
#include <engine/typing/class_registry.hpp>

// Sapphire Engine Material Definition
// By default semd is treated as text, binary forms are "semdb"
class MaterialLoader : public AssetLoader {
    REFLECT_CLASS(MaterialLoader, AssetLoader);

protected:
    void register_classes() override;

    void release_cache() override;

public:
    std::vector<std::string> get_extensions() override;

    std::shared_ptr<Asset> load_from_path(const std::string &path, const std::string& extension) override;
};


#endif
