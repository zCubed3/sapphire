#ifndef SAPPHIRE_VULKAN_TEXTURE_H
#define SAPPHIRE_VULKAN_TEXTURE_H

#include <engine/rendering/texture.h>

class ValImage;

class VulkanTexture : public Texture {
public:
    ValImage *val_image = nullptr;

    ~VulkanTexture() override;

    void load_bytes(unsigned char *bytes, int width, int height, int channels) override;
};


#endif
