#include "glsl_shader_asset_loader.h"

#include <rs_opengl4/assets/glsl_shader_asset.h>

#include <fstream>

void GLSLShaderAssetLoader::load_placeholders() {
    GLSLShaderAsset::get_placeholder();
}

std::vector<std::string> GLSLShaderAssetLoader::get_extensions() {
    return {"glsl"};
}

Asset *GLSLShaderAssetLoader::load_from_path(const std::string &path, const std::string& extension) {
    // Verify the shader actually exists first
    // TODO: Smarter verification
    // TODO: OS utilities?
    std::ifstream file(path);

    if (file.is_open()) {
        file.close();
        return new GLSLShaderAsset(path);
    }

    return nullptr;
}