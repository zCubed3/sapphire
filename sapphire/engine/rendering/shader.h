#ifndef SAPPHIRE_SHADER_H
#define SAPPHIRE_SHADER_H

class ConfigFile;

class TextureAsset;

// A shader is the underlying program of a material
class Shader {
protected:
    enum CullMode {
        CULL_MODE_BACK,
        CULL_MODE_FRONT,
        CULL_MODE_NONE
    };

    enum DepthOp {
        DEPTH_OP_LESS,
        DEPTH_OP_LESS_OR_EQUAL,
        DEPTH_OP_GREATER,
        DEPTH_OP_GREATER_OR_EQUAL,
        DEPTH_OP_EQUAL,
        DEPTH_OP_ALWAYS
    };

    bool write_depth = true;
    CullMode cull_mode = CullMode::CULL_MODE_BACK;
    DepthOp depth_op = DepthOp::DEPTH_OP_LESS;

public:
    TextureAsset* texture_asset = nullptr;

    virtual ~Shader();

    virtual bool make_from_semd(ConfigFile *p_semd_file);
};

#endif
