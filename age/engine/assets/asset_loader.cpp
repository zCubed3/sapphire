#include "asset_loader.h"

#include <engine/assets/loaders/mmdl_loader.h>
#include <engine/assets/loaders/obj_loader.h>

std::vector<AssetLoader *> AssetLoader::loaders = {};

void AssetLoader::register_engine_asset_loaders() {
    register_loader<OBJLoader>();
    register_loader<MMDLLoader>();
}
