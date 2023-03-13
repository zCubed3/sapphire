#include "view_buffer.hpp"

#include <engine/rendering/buffers/graphics_buffer.hpp>

ViewBuffer::ViewBuffer(GraphicsBuffer *p_buffer) {
    buffer = p_buffer;
}

ViewBuffer::~ViewBuffer() {
    delete buffer;
}

void ViewBuffer::write(ViewBufferData data) {
    buffer->write(&data, sizeof(ViewBufferData));
}