#ifndef SAPPHIRE_GRAPHICS_BUFFER_H
#define SAPPHIRE_GRAPHICS_BUFFER_H

class GraphicsBuffer {
public:
    virtual ~GraphicsBuffer();

    virtual void write(void* data, size_t size) = 0;
};

#endif
