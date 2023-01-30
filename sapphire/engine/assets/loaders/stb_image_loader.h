#ifndef SAPPHIRE_STB_IMAGE_LOADER_H
#define SAPPHIRE_STB_IMAGE_LOADER_H

#include <engine/assets/asset_loader.h>

// Loads image assets using stb_image.h
class STBImageLoader : public AssetLoader {
    REFLECT_CLASS(STBImageLoader, AssetLoader)

public:
    std::vector<std::string> get_extensions() override;

    Asset * load_from_path(const std::string &path, const std::string &extension) override;
};


#endif
