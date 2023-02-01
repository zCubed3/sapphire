#ifndef SAPPHIRE_TEXTURE_H
#define SAPPHIRE_TEXTURE_H

class Texture {
public:
    virtual ~Texture() = default;

#if defined(IMGUI_SUPPORT)
    virtual void* get_imgui_handle() = 0;
#endif

    virtual void load_bytes(unsigned char* bytes, int width, int height, int channels) = 0;
};

#endif
