#ifndef SAPPHIRE_VAL_SHADER_MODULE_H
#define SAPPHIRE_VAL_SHADER_MODULE_H

#include <rs_vulkan/val/val_releasable.h>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

struct ValShaderModuleCreateInfo {
    enum Stage {
        STAGE_VERTEX,
        STAGE_FRAGMENT
    };

    std::vector<char> code;
    Stage stage = Stage::STAGE_VERTEX;
    std::string entry_point = "";
};

class ValShaderModule : public ValReleasable {
public:
    VkShaderModule vk_shader_module = nullptr;
    VkPipelineShaderStageCreateInfo vk_stage_info;
    ValShaderModuleCreateInfo::Stage stage;

    static ValShaderModule* create_shader_module(ValShaderModuleCreateInfo *p_create_info, ValInstance *p_val_instance);
};

#endif
