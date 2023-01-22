#ifndef AGE_GLSL_SHADER_H
#define AGE_GLSL_SHADER_H

#include <engine/assets/shader_asset.h>

#include <unordered_map>

class GLSLShaderAsset : public ShaderAsset {
protected:
    static GLSLShaderAsset *placeholder;
    static uint32_t create_shader(uint32_t type, const std::vector<const char *> &sources);
    static std::string get_shader_error(uint32_t handle);

    // TODO: Make this editor only?
    std::string source_path;

    std::unordered_map<std::string, uint32_t> uniform_cache = {};
    uint32_t get_uniform(const std::string &var);

public:
    uint32_t shader_handle = 0;

    // GLSL shaders compile through the driver
    // Therefore the loader is just a filter

    GLSLShaderAsset();
    GLSLShaderAsset(const std::string &path);

    // TODO: Error reporting
    void compile_from_disk();
    void compile_source(const std::string &source);

    void set_float(const std::string &var, float val) override;
    void set_vec2(const std::string &var, const glm::vec2 &val) override;
    void set_vec3(const std::string &var, const glm::vec3 &val) override;
    void set_vec4(const std::string &var, const glm::vec4 &val) override;
    void set_mat4(const std::string &var, const glm::mat4 &val) override;

    static GLSLShaderAsset *get_placeholder();
};

#endif
