#ifndef SAPPHIRE_GRAPHICS_BUFFER_H
#define SAPPHIRE_GRAPHICS_BUFFER_H

#include <cstdlib>

class GraphicsBuffer {
public:
    virtual ~GraphicsBuffer();

    virtual void write(void* data, size_t size) = 0;
};

#endif
