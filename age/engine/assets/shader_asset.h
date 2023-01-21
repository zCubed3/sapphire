#ifndef AGE_SHADER_H
#define AGE_SHADER_H

#include <engine/assets/asset.h>
#include <glm.hpp>
#include <string>

class ShaderAsset : public Asset {
public:
    virtual void set_float(const std::string &var, float val) = 0;
    virtual void set_vec2(const std::string &var, const glm::vec2 &val) = 0;
    virtual void set_vec3(const std::string &var, const glm::vec3 &val) = 0;
    virtual void set_vec4(const std::string &var, const glm::vec4 &val) = 0;
    virtual void set_mat4(const std::string &var, const glm::mat4 &val) = 0;
};

#endif
