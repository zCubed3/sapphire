#include "view_buffer.h"

#include <engine/rendering/graphics_buffer.h>

ViewBuffer::ViewBuffer(GraphicsBuffer *p_buffer) {
    buffer = p_buffer;
}

void ViewBuffer::write(ViewBufferData data) {
    buffer->write(&data, sizeof(ViewBufferData));
}