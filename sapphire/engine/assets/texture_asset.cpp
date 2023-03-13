#include "texture_asset.hpp"

#include <engine/rendering/texture.hpp>

TextureAsset::~TextureAsset() {
    delete texture;
}
