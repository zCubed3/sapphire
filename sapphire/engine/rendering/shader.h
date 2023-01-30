#ifndef SAPPHIRE_SHADER_H
#define SAPPHIRE_SHADER_H

#include <string>
#include <unordered_map>

class ConfigFile;

class TextureAsset;

// A shader is the underlying program of a material
class Shader {
protected:
    static std::unordered_map<std::string, Shader*> shader_cache;

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
    std::string name;
    TextureAsset* texture_asset = nullptr;

    static Shader* get_cached_shader(const std::string& name);
    static void cache_shader(Shader* shader);

    virtual ~Shader() = default;

    virtual bool make_from_sesd(ConfigFile *p_sesd_file);
};

#endif
