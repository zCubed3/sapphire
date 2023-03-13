#ifndef SAPPHIRE_MATERIAL_ASSET_HPP
#define SAPPHIRE_MATERIAL_ASSET_HPP

#include <engine/assets/asset.hpp>
#include <glm.hpp>
#include <memory>
#include <string>

class Shader;
class Material;
class TextureAsset;

class MaterialAsset : public Asset {
    REFLECT_CLASS(MaterialAsset, Asset)

public:
    std::shared_ptr<Material> material = nullptr;

    ~MaterialAsset() override;
};

#endif
