#ifndef SAPPHIRE_GLSL_SHADER_LOADER_H
#define SAPPHIRE_GLSL_SHADER_LOADER_H

#include <engine/assets/asset_loader.h>

class GLSLShaderAssetLoader : public AssetLoader {
protected:
    void load_placeholders() override;

public:
    std::vector<std::string> get_extensions() override;

    Asset *load_from_path(const std::string &path) override;
};

#endif
