#include "asset_loader.h"

#include <iostream>

#include <engine/assets/loaders/mmdl_loader.h>
#include <engine/assets/loaders/obj_loader.h>

std::vector<AssetLoader *> AssetLoader::loaders = {};

void AssetLoader::register_engine_asset_loaders() {
    register_loader<OBJLoader>();
    register_loader<MMDLLoader>();
}

Asset *AssetLoader::load_asset(const std::string &path) {
    // TODO: Make this smarter and safer
    size_t last_period = path.find_last_of('.');
    std::string extension = path.substr(last_period + 1);

    for (AssetLoader *loader: loaders) {
        if (loader != nullptr) {
            for (const std::string &expected: loader->get_extensions()) {
                if (extension == expected) {
                    return loader->load_from_path(path);
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