#include "asset_loader.h"

#include <iostream>

#include <engine/assets/loaders/obj_loader.h>
#include <engine/assets/loaders/semd_loader.h>
#include <engine/assets/loaders/stb_image_loader.h>

#include <engine/assets/asset.h>
#include <engine/assets/texture_asset.h>
#include <engine/assets/material_asset.h>

std::vector<AssetLoader *> AssetLoader::loaders = {};

void AssetLoader::load_placeholders() {

}

void AssetLoader::unload_placeholders() {

}

void AssetLoader::register_engine_asset_loaders() {
    register_loader<OBJLoader>();
    register_loader<SEMDLoader>();
    register_loader<STBImageLoader>();

    ClassRegistry::register_class<Asset>();
    ClassRegistry::register_class<TextureAsset>();
    ClassRegistry::register_class<MaterialAsset>();
}

Asset *AssetLoader::load_asset(const std::string &path) {
    // TODO: Make this smarter and safer
    size_t last_period = path.find_last_of('.');
    std::string extension = path.substr(last_period + 1);

    for (AssetLoader *loader: loaders) {
        if (loader != nullptr) {
            for (const std::string &expected: loader->get_extensions()) {
                if (extension == expected) {
                    return loader->load_from_path(path, extension);
                }
            }
        }
    }

    return nullptr;
}

void AssetLoader::load_all_placeholders() {
    for (AssetLoader *loader: loaders) {
        if (loader != nullptr) {
            loader->load_placeholders();
        }
    }
}

void AssetLoader::unload_all_placeholders() {
    for (AssetLoader *loader: loaders) {
        if (loader != nullptr) {
            loader->unload_placeholders();
        }
    }
}
