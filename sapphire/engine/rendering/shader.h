#ifndef SAPPHIRE_SHADER_H
#define SAPPHIRE_SHADER_H

class ConfigFile;

class TextureAsset;

// A shader is the underlying program of a material
class Shader {
public:
    TextureAsset* texture_asset = nullptr;

    virtual ~Shader();

    virtual bool make_from_semd(ConfigFile *p_semd_file) = 0;
};

#endif
