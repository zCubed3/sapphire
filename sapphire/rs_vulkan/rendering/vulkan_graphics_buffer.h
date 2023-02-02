#ifndef SAPPHIRE_VULKAN_GRAPHICS_BUFFER_H
#define SAPPHIRE_VULKAN_GRAPHICS_BUFFER_H

#include <engine/rendering/buffers/graphics_buffer.h>

class ValBuffer;
class ValDescriptorSet;

class VulkanGraphicsBuffer : public GraphicsBuffer {
public:
    ValBuffer *val_buffer = nullptr;
    ValDescriptorSet *val_descriptor_set = nullptr;

    VulkanGraphicsBuffer(size_t size);
    ~VulkanGraphicsBuffer() override;

    void write(void *data, size_t size) override;
};


#endif
