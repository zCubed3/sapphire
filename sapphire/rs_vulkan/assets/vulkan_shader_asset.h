#ifndef SAPPHIRE_VULKAN_SHADER_H
#define SAPPHIRE_VULKAN_SHADER_H

#include <engine/assets/shader_asset.h>

#include <unordered_map>

// 
class VulkanShaderBinary {

};

class VulkanShaderAsset : public ShaderAsset {
protected:
    static VulkanShaderAsset *placeholder;

public:
    VulkanShaderAsset();
    VulkanShaderAsset(const std::string& path);

    void load(char* bytes, size_t length);

    void set_float(const std::string &var, float val) override;
    void set_vec2(const std::string &var, const glm::vec2 &val) override;
    void set_vec3(const std::string &var, const glm::vec3 &val) override;
    void set_vec4(const std::string &var, const glm::vec4 &val) override;
    void set_mat4(const std::string &var, const glm::mat4 &val) override;

    static VulkanShaderAsset *get_placeholder();
};

#endif
