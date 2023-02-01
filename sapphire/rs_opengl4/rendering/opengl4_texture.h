#ifndef SAPPHIRE_OPENGL4_TEXTURE_H
#define SAPPHIRE_OPENGL4_TEXTURE_H

#include <cstdint>
#include <engine/rendering/texture.h>

class OpenGL4Texture : public Texture {
public:
    bool owns_handle = true;
    uint32_t handle = -1;

    OpenGL4Texture() = default;
    OpenGL4Texture(uint32_t handle, bool owns_handle = true);
    ~OpenGL4Texture() override;

    void * get_imgui_handle() override;
    void load_bytes(unsigned char *bytes, int width, int height, int channels) override;
};


#endif
