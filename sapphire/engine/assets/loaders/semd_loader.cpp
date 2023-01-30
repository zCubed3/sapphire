#include "semd_loader.h"

#include <fstream>

#include <engine/assets/shader_asset.h>
#include <engine/config/config_file.h>
#include <engine/rendering/render_server.h>
#include <engine/rendering/shader.h>

std::vector<std::string> SEMDLoader::get_extensions() {
    return {"semd", "bsemd"};
}

Asset *SEMDLoader::load_from_path(const std::string &path, const std::string& extension) {
    const RenderServer* rs_instance = RenderServer::get_singleton();

    if (rs_instance == nullptr) {
        return nullptr;
    }

    if (extension == "semd") {
        // SEMD files are just config files
        ConfigFile file;
        file.read_from_path(path);

        // We then create a shader
        Shader* shader = rs_instance->create_shader();
        shader->make_from_semd(&file);

        ShaderAsset *asset = new ShaderAsset();
        asset->shader = shader;

        return asset;
    }

    if (extension == "bsemd") {
        // TODO
    }

    return nullptr;
}
