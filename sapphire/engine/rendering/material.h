#ifndef SAPPHIRE_MATERIAL_H
#define SAPPHIRE_MATERIAL_H

#include <engine/rendering/shader.h>

class ConfigFile;
class Shader;

class Material {
public:
    std::vector<Shader::ShaderParameter> parameter_overrides;

    Shader *shader;

    virtual ~Material() = default;

    virtual bool make_from_semd(ConfigFile *p_semd_file);

    virtual void bind() = 0;
};

#endif
