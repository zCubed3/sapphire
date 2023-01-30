#ifndef SAPPHIRE_TEXTURE_ASSET_H
#define SAPPHIRE_TEXTURE_ASSET_H

#include <engine/assets/asset.h>

class Texture;

class TextureAsset : public Asset {
public:
    Texture *texture = nullptr;
};

#endif
