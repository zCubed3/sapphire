#ifndef SAPPHIRE_SHADER_H
#define SAPPHIRE_SHADER_H

class ConfigFile;

// A shader is the underlying program of a material
class Shader {
public:
    virtual ~Shader();

    virtual bool make_from_semd(ConfigFile *p_semd_file) = 0;
};

#endif
