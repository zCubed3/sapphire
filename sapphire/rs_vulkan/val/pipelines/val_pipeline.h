#ifndef SAPPHIRE_VAL_PIPELINE_H
#define SAPPHIRE_VAL_PIPELINE_H

#include <rs_vulkan/val/val_releasable.h>
#include <vulkan/vulkan.h>

class ValPipeline : public ValReleasable {
public:
    VkPipeline vk_pipeline = nullptr;
    VkPipelineLayout vk_pipeline_layout = nullptr;

    void release(ValInstance *p_val_instance) override;
};


#endif
