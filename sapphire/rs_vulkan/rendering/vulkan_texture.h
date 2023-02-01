#ifndef SAPPHIRE_VULKAN_TEXTURE_H
#define SAPPHIRE_VULKAN_TEXTURE_H

#include <engine/rendering/texture.h>

class ValImage;
class ValDescriptorSet;

class VulkanTexture : public Texture {
protected:
#if defined(IMGUI_SUPPORT)
    ValDescriptorSet *val_imgui_descriptor_set = nullptr;
#endif
    bool owns_image = true;

public:
    ValImage *val_image = nullptr;

    VulkanTexture() = default;
    VulkanTexture(ValImage *val_image, bool owns_image = true);
    ~VulkanTexture() override;

#if defined(IMGUI_SUPPORT)
    void * get_imgui_handle() override;
#endif

    void load_bytes(unsigned char *bytes, int width, int height, int channels) override;
};


#endif
