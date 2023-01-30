#ifndef SAPPHIRE_OPENGL4_SHADER_H
#define SAPPHIRE_OPENGL4_SHADER_H

#include <engine/rendering/shader.h>

#include <unordered_map>

// fyi, GLSL shaders compile through the driver!
class OpenGL4Shader : public Shader {
protected:
    static uint32_t create_shader(uint32_t type, const std::vector<const char *> &sources);
    static std::string get_shader_error(uint32_t handle);
    static std::string get_program_error(uint32_t handle);

    std::unordered_map<std::string, uint32_t> uniform_cache = {};

public:
    static OpenGL4Shader *error_shader;

    uint32_t shader_handle = 0;

    uint32_t get_uniform(const std::string &var);
    uint32_t get_uniform_block(const std::string &var);

    bool make_from_sesd(ConfigFile *p_sesd_file) override;

    // TODO: Error reporting
    void compile_sources(const std::string &vert_source, const std::string &frag_source);

    static void create_error_shader();
};

#endif
