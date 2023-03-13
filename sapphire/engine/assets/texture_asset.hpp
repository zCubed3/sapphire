#ifndef SAPPHIRE_TEXTURE_ASSET_HPP
#define SAPPHIRE_TEXTURE_ASSET_HPP

#include <engine/assets/asset.hpp>

class Texture;

class TextureAsset : public Asset {
    REFLECT_CLASS(TextureAsset, Asset)

public:
    Texture *texture = nullptr;

    ~TextureAsset() override;
};

#endif
