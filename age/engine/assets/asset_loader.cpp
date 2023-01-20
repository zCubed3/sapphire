#include "asset_loader.h"

#include <engine/assets/loaders/mesh_loader.h>


void AssetLoader::register_engine_loaders() {
    register_loader<MeshLoader>();
}
