#ifndef SAPPHIRE_TEXTURE_H
#define SAPPHIRE_TEXTURE_H

class Texture {
public:
    enum Dimensions {
        DIMENSIONS_2D,
        DIMENSIONS_3D,
        DIMENSIONS_CUBE
    };

protected:
    Dimensions dimensions = DIMENSIONS_2D;

public:
    virtual Dimensions get_dimensions() const;

    virtual ~Texture() = default;

#if defined(IMGUI_SUPPORT)
    virtual void* get_imgui_handle() = 0;
#endif

    virtual void load_bytes(unsigned char* bytes, int width, int height, int channels) = 0;
};

#endif
