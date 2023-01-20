#include "glsl_shader_asset_loader.h"

#include <fstream>

std::vector<std::string> GLSLShaderAssetLoader::get_extensions() {
    return {"glsl"};
}

Asset *GLSLShaderAssetLoader::load_from_path(const std::string &path) {
    std::ifstream file(path);

    if (file.is_open()) {

    }

    return nullptr;
}