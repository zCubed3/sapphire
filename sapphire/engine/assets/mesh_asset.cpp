#include "mesh_asset.h"

#include <engine/rendering/mesh_buffer.h>

MeshAsset::~MeshAsset() {
    delete buffer;
}