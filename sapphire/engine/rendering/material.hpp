#ifndef SAPPHIRE_MATERIAL_HPP
#define SAPPHIRE_MATERIAL_HPP

#include <engine/rendering/shader.hpp>

class ConfigFile;
class Shader;

class Material {
public:
    std::vector<Shader::ShaderParameter> parameter_overrides;

    std::shared_ptr<Shader> shader;

    virtual ~Material() = default;

    virtual bool make_from_semd(ConfigFile *p_semd_file);

    virtual bool bind_material(ShaderPass *p_shader_pass) = 0;
};

#endif
