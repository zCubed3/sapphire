#include "semd_loader.h"

#include <fstream>

#include <engine/assets/shader_asset.h>
#include <engine/config/config_file.h>
#include <engine/platforms/platform.h>
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
        ConfigFile semd_file;
        semd_file.read_from_path(path);

        std::string sesd_path = semd_file.try_get_string("sSESDPath", "Shader");
        if (!Platform::get_singleton()->file_exists(sesd_path)) {
            return nullptr;
        }

        ConfigFile sesd_file;
        sesd_file.read_from_path(sesd_path);

        // We then create a shader
        // Check if the referenced SESD file has been loaded before
        // TODO: Use smart pointers for asset references
        std::string sesd_name = sesd_file.try_get_string("sName", "Shader");

        if (sesd_name.empty()) {
            return nullptr;
        }

        Shader* shader = Shader::get_cached_shader(sesd_name);
        if (shader == nullptr) {
            shader = rs_instance->create_shader();
            shader->make_from_sesd(&sesd_file);

            Shader::cache_shader(shader);
        }

        MaterialAsset *asset = new MaterialAsset();
        asset->shader = shader;

        return asset;
    }

    if (extension == "bsemd") {
        // TODO
    }

    return nullptr;
}
