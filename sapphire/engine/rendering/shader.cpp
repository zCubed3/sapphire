#include "shader.h"

#include <engine/config/config_file.h>

std::unordered_map<std::string, Shader*> Shader::shader_cache = {};

void Shader::release_cache() {
    for (auto pair: shader_cache) {
        delete pair.second;
    }
}

Shader *Shader::get_cached_shader(const std::string &name) {
    auto iter = shader_cache.find(name);

    if (iter != shader_cache.end()) {
        return iter->second;
    }

    return nullptr;
}

void Shader::cache_shader(Shader *shader) {
    if (shader == nullptr) {
        return;
    }

    shader_cache.emplace(shader->name, shader);
}

bool Shader::make_from_sesd(ConfigFile *p_sesd_file) {
    if (p_sesd_file == nullptr) {
        return false;
    }

    // TODO: Caseless comparisons
    write_depth = p_sesd_file->try_get_int("bWriteDepth", "Material", 1);

    std::string cull_string = p_sesd_file->try_get_string("sCullMode", "Material", "Back");

    if (cull_string == "Back") {
        cull_mode = CULL_MODE_BACK;
    } else if (cull_string == "Front") {
        cull_mode = CULL_MODE_FRONT;
    } else {
        cull_mode = CULL_MODE_NONE;
    }

    std::string depth_string = p_sesd_file->try_get_string("sDepthOp", "Material", "Less");

    if (depth_string == "Less") {
        depth_op = DEPTH_OP_LESS;
    } else if (depth_string == "LessOrEqual") {
        depth_op = DEPTH_OP_LESS_OR_EQUAL;
    } else if (depth_string == "Greater") {
        depth_op = DEPTH_OP_GREATER;
    } else if (depth_string == "GreaterOrEqual") {
        depth_op = DEPTH_OP_GREATER_OR_EQUAL;
    } else if (depth_string == "Equal") {
        depth_op = DEPTH_OP_EQUAL;
    } else {
        depth_op = DEPTH_OP_ALWAYS;
    }

    std::vector<std::string> texture_params = p_sesd_file->try_get_string_list("aTextureParameters", "Shader");

    if (!texture_params.empty() && texture_params.size() % 2 == 0) {
        for (int t = 0; t < texture_params.size(); t += 2) {
            ShaderParameter parameter {};
            parameter.location = atoi(texture_params[t].c_str());
            parameter.name = texture_params[t + 1];

            parameters.push_back(parameter);
        }
    }

    return true;
}
