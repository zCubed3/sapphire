#ifndef SAPPHIRE_TEXTURE_H
#define SAPPHIRE_TEXTURE_H

class Texture {
public:
    virtual void load_bytes(unsigned char* bytes, int width, int height, int channels) = 0;
};

#endif
