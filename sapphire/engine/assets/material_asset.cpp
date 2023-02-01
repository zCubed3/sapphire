#include "material_asset.h"

#include <engine/rendering/material.h>

MaterialAsset::~MaterialAsset() {
    delete material;
}