#ifndef SAPPHIRE_SEMD_LOADER_H
#define SAPPHIRE_SEMD_LOADER_H

#include <engine/assets/asset_loader.h>
#include <engine/typing/class_registry.h>

// Sapphire Engine Material Definition
// By default semd is treated as text, binary forms are "semdb"
class SEMDLoader : public AssetLoader {
    REFLECT_CLASS(SEMDLoader, AssetLoader);

public:
    std::vector<std::string> get_extensions() override;

    Asset * load_from_path(const std::string &path, const std::string& extension) override;
};


#endif
