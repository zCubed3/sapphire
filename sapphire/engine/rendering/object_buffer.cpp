#include "object_buffer.h"

ObjectBuffer::ObjectBuffer(GraphicsBuffer *p_buffer) {
    buffer = p_buffer;
}

ObjectBuffer::~ObjectBuffer() {
    delete buffer;
}

void ObjectBuffer::write(ObjectBufferData data) {
    buffer->write(&data, sizeof(ObjectBufferData));
}