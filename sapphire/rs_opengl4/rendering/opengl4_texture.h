#ifndef SAPPHIRE_OPENGL4_TEXTURE_H
#define SAPPHIRE_OPENGL4_TEXTURE_H

#include <cstdint>
#include <engine/rendering/texture.h>

class OpenGL4Texture : public Texture {
public:
    uint32_t handle = -1;

    void load_bytes(unsigned char *bytes, int width, int height, int channels) override;
};


#endif
