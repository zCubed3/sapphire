#ifndef SAPPHIRE_SHADER_ASSET_H
#define SAPPHIRE_SHADER_ASSET_H

#include <engine/assets/asset.h>
#include <glm.hpp>
#include <memory>
#include <string>

class Shader;

class MaterialAsset : public Asset {
public:
    Shader* shader = nullptr;
};

#endif
