#ifndef AGE_GLSL_SHADER_H
#define AGE_GLSL_SHADER_H

#include <engine/assets/shader_asset.h>

#include <unordered_map>

class GLSLShaderAsset : public ShaderAsset {
protected:
    std::unordered_map<std::string, uint32_t> uniform_cache = {};

    uint32_t get_uniform(const std::string& var);

public:
    uint32_t shader_handle = 0;

    void set_float(const std::string &var, float val) override;
};

#endif
