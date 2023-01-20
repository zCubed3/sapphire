#include "glsl_shader_asset.h"

#include <glad/glad.h>

uint32_t GLSLShaderAsset::get_uniform(const std::string &var) {
    auto iter = uniform_cache.find(var);

    if (iter == uniform_cache.end()) {
        uint32_t loc = glGetUniformLocation(shader_handle, var.c_str());

        // Regardless of whether it exists we cache it
        // When setting is when the index is checked
        uniform_cache.emplace(var, loc);
    }

    return iter->second;
}