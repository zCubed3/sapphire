#ifndef AGE_GLSL_SHADER_H
#define AGE_GLSL_SHADER_H

#include <engine/assets/asset_loader.h>

class GLSLShaderAssetLoader : public AssetLoader {
public:
    std::vector<std::string> get_extensions() override;

    Asset * load_from_path(const std::string &path) override;
};

#endif
