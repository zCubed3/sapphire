#ifndef SAPPHIRE_VAL_VERTEX_INPUT_BUILDER_H
#define SAPPHIRE_VAL_VERTEX_INPUT_BUILDER_H

#include <vector>
#include <vulkan/vulkan.h>

// Helps the user construct pipeline vertex input structures easier
class ValVertexInputBuilder {
protected:
    std::vector<VkVertexInputAttributeDescription> attribute_descriptions {};
    uint32_t total_size = 0;

public:
    enum AttributeDataType {
        // TODO: Other data types?
        ATTRIBUTE_DATA_TYPE_FLOAT,
        ATTRIBUTE_DATA_TYPE_VEC2,
        ATTRIBUTE_DATA_TYPE_VEC3,
        ATTRIBUTE_DATA_TYPE_VEC4
    };

    void push_attribute(AttributeDataType type);

    VkVertexInputBindingDescription get_binding_description() const;
    std::vector<VkVertexInputAttributeDescription> get_input_attributes() const;
};


#endif
