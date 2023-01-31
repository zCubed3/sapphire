#ifndef SAPPHIRE_MATERIAL_ASSET_H
#define SAPPHIRE_MATERIAL_ASSET_H

#include <engine/assets/asset.h>
#include <glm.hpp>
#include <memory>
#include <string>

class Shader;
class Material;
class TextureAsset;

class MaterialAsset : public Asset {
public:
    Material* material = nullptr;
};

#endif
