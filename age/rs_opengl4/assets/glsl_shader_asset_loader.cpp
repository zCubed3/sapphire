#include "glsl_shader_asset_loader.h"

std::vector<std::string> GLSLShaderAssetLoader::get_extensions() {
    return { ".glsl" };
}

Asset *GLSLShaderAssetLoader::load_from_path(const std::string &path) {
    return nullptr;
}